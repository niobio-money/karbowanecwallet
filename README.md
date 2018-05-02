**1. Clone wallet sources**

```
git clone https://github.com/niobio-cash/niobio-wallet.git
git submodule update --init
```

**2. Install the dependencies**
- g++
- cmake
- boost
- qt5
- qttools5 <only needed for generate/update .qm language files>

On Ubuntu:
```
sudo apt-get install qt5-default qttools5-dev-tools cmake libboost-all-dev g++
```

**3. Build**

```
mkdir build && cd build && cmake .. && make
```

