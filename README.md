# English
## Building Niobio Cash GUI Wallet

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
# Português
## Compilando Carteira Gráfica Niobio Cash

**1. Clonar fontes da Carteira**

```
git clone https://github.com/niobio-cash/niobio-wallet.git
git submodule update --init
```

**2. Instalar as Dependências**
- g++
- cmake
- boost
- qt5
- qttools5 <somente necessária para gerar/atualizar arquivos de linguagem .qm>

No Ubuntu:
```
sudo apt-get install qt5-default qttools5-dev-tools cmake libboost-all-dev g++
```

**3. Compilar**

```
mkdir build && cd build && cmake .. && make
```

