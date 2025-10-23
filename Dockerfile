# mameGC Docker Build Environment
FROM devkitpro/devkitppc:20240612

# Keep image lean
RUN rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . /workspace/
CMD ["bash"]
