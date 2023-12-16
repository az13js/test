FROM alpine:3.16 AS build_images

COPY . /opt/
WORKDIR /opt/

ENV CXXFLAGS "-DCEPHALOPOD_NO_IPV6_SUPPORT"
ENV LDFLAGS "-static"

RUN apk add -U -v --no-cache --no-progress g++ gcc make && make

FROM alpine:3.16

COPY --from=build_images /opt/proxy /bin/proxy
RUN mkdir /opt/cache
WORKDIR /opt/

CMD ["proxy", "--listen-ip=0.0.0.0", "--listen-port=80"]
