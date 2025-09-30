FROM alpine:latest

RUN apk --no-cache add build-base openssl-dev make cmake

RUN addgroup -g 1001 stego \
    && adduser -D -u 1001 -G stego stego \
    && mkdir -p /usr/app \
    && chown -R stego:stego /usr/app

WORKDIR /usr/app
USER 1001

COPY --chown=1001:1001 . .
RUN chmod +x run.sh

# RUN mkdir build && cd build && cmake .. && make

CMD ["sh", "run.sh"]
