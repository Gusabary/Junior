#!/bin/sh
java -jar ./zkwatcher.jar &
mongod --bind_ip 0.0.0.0