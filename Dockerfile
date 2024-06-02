FROM alpine:latest

RUN apk add --update alpine-sdk libc6-compat

COPY avr8-gnu-toolchain-3.7.0.1796-linux.any.x86_64.tar.gz /opt/avr8-gnu-toolchain-3.7.0.1796-linux.any.x86_64.tar.gz

RUN cd /opt/ && \
    tar -xf avr8-gnu-toolchain-3.7.0.1796-linux.any.x86_64.tar.gz && \
    rm /opt/avr8-gnu-toolchain-3.7.0.1796-linux.any.x86_64.tar.gz

WORKDIR /usr/app/
#ENTRYPOINT [ "/bin/sh" ]
