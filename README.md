## app-plugin-boilerplate

This repo is meant to be a forkable example of a plugin.

Plugins are lightweight applications that go hand-in-hand with the Stellar
Application on a Nano (S, S plus, X) and Stax devices.

They allow users to safely interact with smart contracts by parsing the
transaction data and displaying its content in a human-readable way. This is
done on a "per contract" basis, meaning a plugin is required for every DApp.

This repo implements a plugin for Stellar Soroban token, where you can find the explanation of the token interface [here](https://developers.stellar.org/docs/smart-contracts/tokens/token-interface).

The code has been commented, developers are expected to adapt the code to their own needs.

## Stellar Plugin SDK

Stellar plugins need the [Stellar Plugin SDK](https://github.com/lightsail-network/libstellar), by default, the `Makefile` expects it to be at the root directory of this plugin repository by the `stellar_plugin_sdk` name.