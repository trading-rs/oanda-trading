#!/usr/bin/env stack
-- stack --resolver lts-8.10 --install-ghc runhaskell --package MissingH -- -Wall
module Main where

import Control.Monad
import System.Directory
import System.Process
import System.Exit

conanPathPrefix :: String
conanPathPrefix = "/.conan/data/"

pocoPath :: String
pocoPath = "Poco/1.7.8p3/pocoproject/stable/package"

opensslPath :: String
opensslPath = "OpenSSL/1.0.2l/conan/stable/package"

zlibPath :: String
zlibPath = "zlib/1.2.11/conan/stable/package"

getPath :: String -> String -> IO String
getPath flag lib = do
  home <- getHomeDirectory
  files <- listDirectory $ home ++ conanPathPrefix ++ lib
  case files of
    [hash] -> return $ getPrefix flag ++ home ++ conanPathPrefix ++ lib ++ "/" ++ hash ++ "/" ++ flag
    _      -> error $ "package: " ++ lib ++ " does not exist"
    where getPrefix "include" = "-I"
          getPrefix "lib"     = "-L"
          getPrefix _         = error $ "unrecognized flag: " ++ flag

getInclude :: String -> IO String
getInclude = getPath "include"

getLib :: String -> IO String
getLib = getPath "lib"

baseIncludes :: [String]
baseIncludes = ["-I./include/range-v3/include", "-I./include/json/src", "-I./include/fmt", "-I./include/catch/single_include"]

links :: [String]
links = ["-lssl", "-lcrypto", "-lz", "-lPocoNetSSL", "-lPocoCrypto", "-lPocoNet", "-lPocoZip", "-lPocoUtil", "-lPocoXML", "-lPocoJSON", "-lPocoFoundation"]

dependencies :: [String]
dependencies = [pocoPath, opensslPath, zlibPath]

cc :: String
cc = "c++"

flags :: [String]
flags = ["-Wall", "-std=c++14"]

srcFiles :: [String]
srcFiles = ["./src/api.hpp", "./src/endpoints.hpp", "./main.cc"]

testFiles :: [String]
testFiles = ["./test/test.cc"]

testExec :: String
testExec = "test-exec"

mainExec :: String
mainExec = "main"

main :: IO ()
main = do
  includes <- foldM (appendDep getInclude) baseIncludes dependencies
  libs     <- foldM (appendDep getLib) [] dependencies
  forM_ srcFiles (\src -> putStrLn ("Compiling: " ++ src) >> system (createSrcCmd src includes))
  exits <- forM testFiles (\test -> putStrLn ("Testing: " ++ test) >> system (createTestCmd test includes libs))
  if all (== ExitSuccess) exits
    then void . system $ createMainCmd includes libs
    else error "test fail"
  where appendDep cont        = \b d -> ((b ++) . (: [])) <$> cont d
        createSrcCmd src icds = unwords [ cc
                                        , unwords flags
                                        , unwords icds
                                        , "-c"
                                        , src]
        createTestCmd test icds libs = unwords [ cc
                                               , unwords flags
                                               , unwords icds
                                               , unwords libs
                                               , unwords links
                                               , test
                                               , "-o"
                                               , testExec
                                               , "&&"
                                               , "./" ++ testExec]
        createMainCmd icds libs = unwords [ cc
                                          , unwords flags
                                          , unwords icds
                                          , unwords libs
                                          , unwords links
                                          , mainExec ++ ".o"
                                          , "-o"
                                          , mainExec]
