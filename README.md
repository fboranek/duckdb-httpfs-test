# duckdb-httpfs-test

A test application demonstrating DuckDB usage with the httpfs extension to query Parquet files from S3 storage.

## How to build

```bash
docker build --build-arg VERSION="1.1.0" --tag "fboranek/duckdb-app:1.1.0" .
```

or simply

```bash
./Dockerfile
```

## How to run

Create `.env.local` with your S3 configuration:

```bash
cat > .env.local << 'EOF'
MY_APP_S3_HOST=your-s3-host.com
MY_APP_S3_ACCESS_KEY=your-access-key
MY_APP_S3_SECRET_KEY=your-secret-key
MY_APP_S3_BUCKET=your-bucket-name
EOF
```

Load environment and run:

```bash
docker run --env-file .env.local -it --rm docker.io/fboranek/duckdb-app:1.0.0
```

### Example output

```console
testSelect: Starting DuckDB initialization
testSelect: Creating S3 secrets
testSelect: Listing S3 files in s3://my-httpfs-test using httpfs extension
testSelect: Files in bucket:
s3://my-httpfs-test/file-1.parquet
s3://my-httpfs-test/file-2.parquet
s3://my-httpfs-test/file-3.parquet
testSelect: Total files 3
Success.
```

## Known issues

The package currently lacks static libraries for the httpfs extension. When using `find_package(DuckDB CONFIG REQUIRED)`, CMake may fail with:

```cmake
find_package(DuckDB CONFIG REQUIRED)
```

Error:

```console
CMake Error at /usr/lib/x86_64-linux-gnu/cmake/DuckDB/DuckDBExports.cmake:210 (message):
  The imported target "httpfs_extension" references the file

     "/usr/lib/x86_64-linux-gnu/libhttpfs_extension.a"

  but this file does not exist.  Possible reasons include:

  * The file was deleted, renamed, or moved to another location.

  * An install or uninstall procedure did not complete successfully.

  * The installation package was faulty and contained

     "/usr/lib/x86_64-linux-gnu/cmake/DuckDB/DuckDBExports.cmake"

  but not all the files it references.

Call Stack (most recent call first):
  /usr/lib/x86_64-linux-gnu/cmake/DuckDB/DuckDBConfig.cmake:18 (include)
  CMakeLists.txt:14 (find_package)


-- Configuring incomplete, errors occurred!
```
