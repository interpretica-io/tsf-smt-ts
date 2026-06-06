# Build environment
FROM debian:bookworm

ARG user
ARG uid
ARG gid
ARG docker_gid
ENV USER=${user}
ENV UID=${uid}
ENV GID=${gid}
ENV DOCKER_GID=${docker_gid}

RUN if [ "$DOCKER_GID" != "${GID}" ] ; then groupadd --gid ${DOCKER_GID} docker ; fi

RUN if [ ! -d /home/${USER} ] ; then mkdir -p /home/${USER} && chmod -R 777 /home/${USER} ; fi

# Install basic dependencies & useful tools. libz3-dev gives the Z3 C
# API (z3.h, libz3) that ta_smt links; cvc5 is not in Debian and is
# installed from upstream below. g++ (build-essential) builds the one
# C++ unit, ta_smt_cvc5.cpp.
RUN apt-get update && \
    apt-get install -y \
        autoconf \
        automake \
        bison \
        build-essential \
        ccache \
        cmake \
        curl \
        docker.io \
        file \
        flex \
        gawk \
        libbsd-dev \
        libz3-dev \
        libjansson-dev \
        libexpat1 \
        libexpat1-dev \
        libcurl4-openssl-dev \
        libglib2.0-dev \
        libnetconf2-dev \
        libncurses5 \
        libncurses5-dev \
        libpam0g \
        libpam0g-dev \
        libpcap0.8-dev \
        libpopt-dev \
        libreadline-dev \
        libssh-dev \
        libssh2-1-dev \
        libssl-dev \
        libusb-1.0-0 \
        libusb-1.0-0-dev \
        libxml2 \
        libxml2-dev \
        libxslt1-dev \
        libxslt1.1 \
        libpcre3-dev \
        libyaml-dev \
        m4 \
        meson \
        openssh-server \
        pkg-config \
        perl \
        python3 \
        rsync \
        ssh \
        sudo \
        tshark \
        tcl-dev \
        unzip \
        wget

# cvc5 from its upstream release: Debian has no cvc5 package, and ta_smt
# links its C++ API (cvc5/cvc5.h, -lcvc5) and parser (-lcvc5parser). The
# shared-library release carries include/ and lib/. Pin a version; the
# asset name and layout are the thing to adjust on the first real build
# (see the tsf-smt README). Without this, the cvc5 backend will not
# compile - and the suite would then have only Z3, which it tolerates
# (each group skips an engine the agent lacks).
ARG CVC5_VERSION=1.2.0
RUN set -eux; \
    url="https://github.com/cvc5/cvc5/releases/download/cvc5-${CVC5_VERSION}/cvc5-Linux-x86_64-shared.zip"; \
    cd /tmp && curl -fsSL -o cvc5.zip "$url" && \
    mkdir -p cvc5 && (cd cvc5 && unzip -q ../cvc5.zip && mv */* . 2>/dev/null || true) && \
    cp -r cvc5/include/* /usr/local/include/ && \
    cp -r cvc5/lib/* /usr/local/lib/ && \
    ldconfig && \
    rm -rf /tmp/cvc5 /tmp/cvc5.zip \
    || echo "cvc5 install skipped (adjust CVC5_VERSION/asset); Z3-only build will result"

RUN echo "${USER} ALL=NOPASSWD: ALL" >> /etc/sudoers

RUN if [ "$DOCKER_GID" != "${GID}" ] ; then groupadd --gid ${GID} ${USER} ; fi

RUN useradd --system \
            --gid ${GID} --gid ${DOCKER_GID} \
            --uid ${UID} \
            --shell /bin/bash \
            ${USER}
RUN usermod -a -G ${DOCKER_GID} ${USER}
RUN usermod -a -G docker ${USER}
