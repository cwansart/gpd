# Requirements (Linux, apt based)
* g++
* cmake
* libssl-dev
* openssl
* libboost-all-dev
* libboost-system-dev

# How to compile
Clone the project, change into the gpd directory, create a build folder and compile:

    $ git clone <git repo address>
    $ cd gpd
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release ..
    $ make -j4

# How to run GPD:
There will be a GpdServer executable in the build folder. Run the command as root:

    # ./GpdServer <pathToTmpDhFile> <pathToSslCertificate> <pathToKeyFile>

For example:
    # ./GpdServer /etc/ssl/certs/gpdTmpDh.pem /etc/ssl/certs/gpd.pem /etc/ssl/private/gpd.key

# How to create a TmpDhFile
 To run the server it needs a Diffie-Hellman parameter exchange file, which is the first argument of the GpdServer. To create such a file, run the following command:

    $ openssl dhparam -out dh_param_2048.pem 2048

Your OpenSSL may ship such files already. If so, you can use them as well.
