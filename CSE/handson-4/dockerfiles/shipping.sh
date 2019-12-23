#!/bin/sh
java -jar ./zkwatcher.jar &
/usr/local/bin/java.sh -jar ./app.jar --port=80