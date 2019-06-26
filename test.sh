#!/bin/bash

parameter=$1
echo "---------------------------------"
echo "Test Script startet"
echo "---------------------------------"
echo "> Es wird folgende Datei getestet: $parameter"
echo "> Das Filesystem wird vorbereitet und mit der Datei beschrieben"
echo "> Start Filesystem: Vorbereitung"
fusermount -u mount
rm -rf container.bin
rm -rf log.txt
rm -rf mount
make clean
make
mkdir mount
touch container.bin
echo "> Start Filesystem: Mit Datei beschreiben"
./mkfs.myfs container.bin $parameter
./mount.myfs container.bin log.txt mount
echo "> Navigiere ins Filesystem und gebe den Inhalt aus"
cd mount 
ls -al
echo "> Inhalt der Datei"
cat $parameter
echo "> In die Datei wird nun 'Hello World' geschrieben"
echo "Hello World" > $parameter
cat $paramter
echo "> In die Datei wird nun 'Hello World 2' angehängt"
echo "Hello World 2" >> $parameter
cat $paramter
echo "> Die Datei wird nun gelöscht"
rm -rf $parameter
ls -al
echo "---------------------------------"
echo "Test Script endet"
echo "---------------------------------"

