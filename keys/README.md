

Public Key Exchange:

same for both variations
- 📡 `gnd/PKA.key` --> 🛰 `sat/PKB.key`
- 📡 `gnd/PKB.key` <-- 🛰 `sat/PKA.key`


### `./20211014_planA/` (satellite initiates key exchange)

- 🛰 satellite executes: `kex_ake_initA()`
- 📡 `gnd/ake_senda.txt` <-- 🛰 `sat/ake_senda.txt`
- 📡 ground station executes: `kex_ake_sharedB()`
- 📡 `gnd/ake_sendb.txt` --> 🛰 `sat/ake_sendb.txt`
- 🛰 satellite executes: `kex_ake_sharedA()`

```bash
diff 20211014_planA/gnd/COMMON.key 20211014_planA/sat/COMMON.key
```

### `./20211014_planB/` (ground station initiates key exchange)

- 📡 ground station executes: `kex_ake_initA()`
- 📡 `gnd/ake_senda.txt` --> 🛰 `sat/ake_senda.txt`
- 🛰 satellite executes: `kex_ake_sharedB()`
- 📡 `gnd/ake_sendb.txt` <-- 🛰 `sat/ake_sendb.txt`
- 📡 ground station executes: `kex_ake_sharedA()`

```bash
diff 20211014_planB/gnd/COMMON.key 20211014_planB/sat/COMMON.key
```



