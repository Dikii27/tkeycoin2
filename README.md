Tkeycoin Core 2.0
=====================================

https://tkeycoin.com

## What is Tkeycoin?

TKEY is a peer-to-peer payment system based on the modular implementation of blockchains. The Protocol uses the nChains technology it is a dynamic addition of chains within a single network, supporting the creation of transactions, both within the same chain and between different chains. The number of chains can be changed and unlimited by the algorithm. For storing blocks, instead of (transaction storage: key-value), a PostgreSQL RDBMS is used, which solves the problems of maintaining tens of thousands of connections to the database.

## Tkeycoin Core 1.0

Currently, the binary version of the Tkeycoin software is only available for the stable version of the TKEY 1.0 Protocol, which can be used immediately, see https://tkeycoin.com/en/wallet/

For the easiest deployment of Tkeycoin network 1.0, please use tkeycoin/tkeycoin image file: https://hub.docker.com/u/tkeycoin

## Integration of Tkeycoin 1.0 with your exchange

Connect to the server via ssh and execute commands:

1. ```wget https://tkeycoin.com/tkeycoin-x.x.x.deb```
2. ```sudo apt-get install ./tkeycoin-x.x.x.deb```
3. ```tkeycoind start```


where, x.x.x is the version number of the application, for example, tkeycoin-1.1.6.deb

`Example:` 

1. ```wget https://tkeycoin.com/tkeycoin-1.1.6.deb```
2. ```sudo apt-get install ./tkeycoin-1.1.6.deb```
3. ```tkeycoind start```

The deb-package will automatically configure the Tkeycoin work environment, install the full tkeycoin node on your server, and add Postgresql to work with the Protocol.

After launch, a full node will start sync of blocks to check the synchronization status, enter the following command:

```tkeycoin-cli getblockchaininfo```

Tkeycoin supports all popular RPC bitcoin commands


## Development Process

The new TKEY 2.0 Protocol is currently being actively developed. This repository is a demo version. We will update the repository after each global update. After the release of the stable version of TKEY 2.0, the Tkeycoin team will release documentation for developers: Protocol Assembly, unit tests, manual testing, and recommendations for adding changes.

## Technical features 2.0

- Lightweight and fast server engine;
- Working with sockets directly, HTTP, Websocket, Packet, as well as fast JSON serialization;
- New address managers;
- Running multiple scripts and flexible integration with other programming languages;
- RDBMS (Available with TKEY 1.0);
- Database auto-update systems (Available with TKEY 1.0);
- nChains support (Available with TKEY 1.0);
- High script processing;
- Multi-level logging.


### Tech Specification

- Max Supply: 147 million TKEY
- Target Spacing per block: 10 * 60 
- Block Reward: 100 TKEY
- Mining Algorithm: mPoW
- nChains: 10
- Default nChain: zero
- Subunits: CryptoGramm (CGR)
- Example, 1 TKEY = 100 000 000 CGR, 1 mTKEY = 100 000 CGR, 1 uTKEY = 100 CGR


## About this Project

Creating an innovative TKEY network, we take the best of Bitcoin, Ethereum, DASH, and other alternative currencies, mixing modern concepts and based on the global experience of IBM, Microsoft and a number of other companies and scientific research. New and proprietary features of Tkeycoin are the nChain multi-blockchain technology (unlimited number of chains for scalability and high throughput), RDBMS is used for storing transaction blocks instead of key-value storage, disk space minimization is implemented, UTXO changes are made, indexes in RDBMS are built to reduce disk wear, and algorithms for changing the number of parallel N chains are implemented.

## License

The `Tkeycoin Core 2.0` is open source and available under the terms of the [MIT License](https://github.com/tkeycoin/tkeycoin2/blob/master/LICENSE)
