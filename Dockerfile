# Start from the official Ubuntu 24.04 image
FROM ubuntu:24.04

# Install required packages
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        build-essential \
        llvm \
        git \
        cmake \
        ca-certificates \
        libssl-dev \
        curl \
        xvfb \
        libgstreamer-plugins-base1.0-dev \
        libgstreamer-plugins-bad1.0-dev \
        gstreamer1.0-plugins-base \
        gstreamer1.0-plugins-good \
        gstreamer1.0-plugins-bad \
        gstreamer1.0-plugins-ugly \
        gstreamer1.0-x \
        gstreamer1.0-tools \
        libgles2-mesa-dev \
        mesa-utils \
        sudo

# Clone the projectM repository and build it
RUN git clone --depth 1 https://github.com/projectM-visualizer/projectm.git /tmp/projectm
WORKDIR /tmp/projectm
RUN git submodule update --init --depth 1
RUN mkdir build
WORKDIR /tmp/projectm/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
RUN make -j$(nproc)
RUN make install
WORKDIR /tmp
RUN rm -rf /tmp/projectm

# Get the projectM preset pack
RUN git clone --depth 1 https://github.com/projectM-visualizer/presets-cream-of-the-crop.git /usr/local/share/projectM/presets

# Get the projectM texture pack
RUN git clone --depth 1 https://github.com/projectM-visualizer/presets-milkdrop-texture-pack.git /usr/local/share/projectM/textures

# Clone the gst-projectm repository and build the GStreamer plugin
RUN git clone https://github.com/projectM-visualizer/gst-projectm.git /tmp/gst-projectm
WORKDIR /tmp/gst-projectm
RUN ./setup.sh --auto
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make
RUN mkdir -p /usr/lib/x86_64-linux-gnu/gstreamer-1.0/ && \
    cp build/libgstprojectm.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0/ && \
    rm -rf /tmp/gst-projectm

# Clean up unnecessary packages to reduce image size
RUN apt-get remove -y \
        build-essential \
        git \
        cmake && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Create a working directory for conversion tasks
WORKDIR /app

# Copy the conversion script
COPY convert.sh /app/
RUN chmod +x /app/convert.sh

# Set environment variables
ENV GST_DEBUG=3
ENV PRESETS_DIR=/usr/local/share/projectM/presets
ENV TEXTURES_DIR=/usr/local/share/projectM/textures
ENV XDG_RUNTIME_DIR=/tmp

# Setup for GPU access
ENV LIBGL_ALWAYS_INDIRECT=0
ENV NVIDIA_DRIVER_CAPABILITIES=all
ENV NVIDIA_VISIBLE_DEVICES=all

# Default command
ENTRYPOINT ["/app/convert.sh"]
