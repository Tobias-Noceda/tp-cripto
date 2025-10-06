FROM alpine:latest

RUN apk --no-cache add build-base openssl-dev make cmake gdb valgrind

RUN addgroup -g 1001 stego \
    && adduser -D -u 1001 -G stego stego \
    && mkdir -p /usr/app \
    && chown -R stego:stego /usr/app

WORKDIR /usr/app
USER 1001

# Don't COPY code - it will be mounted as a volume at runtime
# This allows live code updates from your host machine

VOLUME ["/usr/app"]

# Use a shell by default so you can run commands interactively
CMD ["/bin/sh"]
