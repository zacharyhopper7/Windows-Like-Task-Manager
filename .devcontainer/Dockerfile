FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

# Install Qt dependencies
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-l10n-tools \
    libqt6core6 \
    libqt6gui6 \
    libqt6widgets6 \
    cmake \
    ninja-build \
    procps \
    psmisc \
    htop \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install X11 utilities for GUI applications
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    x11-apps \
    xauth \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y libopengl-dev libgl-dev

