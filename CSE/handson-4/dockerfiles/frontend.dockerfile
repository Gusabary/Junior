FROM dplsming/sockshop-frontend:0.2

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./frontend.sh ./frontend.sh

USER root
RUN chmod +x ./frontend.sh
# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "./frontend.sh" ]
# ENTRYPOINT [ "sh", "-c", "npm start" ]

