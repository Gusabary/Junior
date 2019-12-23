#!/bin/sh
java -jar ./zkwatcher.jar &
java -jar ./app.jar --port=80