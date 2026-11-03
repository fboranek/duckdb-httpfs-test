#!/usr/bin/env -S docker build --build-arg VERSION="1.0.0" --tag "fboranek/duckdb-app:1.0.0" . -f

# Stage 1: Build Debian package
FROM debian:forky AS builder

WORKDIR /build

# Copy source code and debian package metadata (debian/ directory with control, rules, etc.)
COPY . .

# Install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        dpkg-dev \
        devscripts \
        fakeroot && \
    apt-get build-dep -y --no-strict-pinning . && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Build the Debian package binary only (-b flag)
# dpkg-buildpackage outputs .deb to parent directory, so we list and verify there
RUN dpkg-buildpackage -b -uc -us && \
    ls -la ../*.deb && \
    dpkg -c ../*.deb


# Stage 2: Minimal runtime image with only the installed package
FROM debian:forky

WORKDIR /app

# Copy only the built .deb package from builder stage
# dpkg-buildpackage outputs to parent of WORKDIR (/build/..) = /
COPY --from=builder /*.deb /tmp/

# Install the package with only runtime dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        /tmp/*.deb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* \
           /tmp/*.deb

ENTRYPOINT [ "/opt/duckdb-app/bin/duckdb-app" ]


