# 🎮 Circuit Escape

Projeto base de jogo 2D desenvolvido em C++ utilizando **SDL2**.
Este projeto cria uma janela com renderer acelerado e implementa um game loop básico — servindo como fundação para futuros sistemas como movimentação, colisão, sprites e mecânicas de gameplay.

---

## 🚀 Tecnologias Utilizadas

* **C++**
* **SDL2**
* **MSYS2 (MinGW64)**
* **g++**

---

## 📦 Pré-requisitos

Antes de rodar o projeto, instale:

* MSYS2
* SDL2 para MinGW64

No terminal **MSYS2 MinGW64**, execute:

```bash
pacman -S mingw-w64-x86_64-SDL2
```

---

## ⚙️ Compilação

Abra o terminal **MSYS2 MinGW64** na pasta do projeto e execute:

```bash
$ g++ *.cpp -o game.exe -lmingw32 -lSDL2main -lSDL2 -mwindows
```

Ou compile e execute em um único comando:

```bash
$ g++ *.cpp -o game.exe -lmingw32 -lSDL2main -lSDL2 -mwindows && ./game.exe
```

---

## ▶️ Executando

Após compilar:

```bash
./game.exe
```

Uma janela será aberta com tela preta (estrutura base do jogo).

---

## 🧠 Estrutura Atual

O projeto atualmente contém:

* Inicialização da SDL
* Criação de janela (128x160 escalado 4x)
* Renderer acelerado
* Game loop com tratamento de eventos
* Limpeza e encerramento correto dos recursos

---

## 📂 Estrutura do Projeto

```
CircuitEscape/
│
├── main.cpp
├── README.md
└── game.exe (gerado após compilação)
```

---

## 🔥 Próximos Passos

* [ ] Movimentação com teclado
* [ ] Sistema de FPS fixo
* [ ] Sprites
* [ ] Colisão
* [ ] Labirinto jogável
* [ ] Sons
* [ ] HUD

---

## 🎯 Objetivo

Este projeto serve como base para:

* Aprendizado de desenvolvimento de jogos 2D
* Criação de engine própria
* Protótipo estilo GameBoy
* Evolução para projetos maiores

---

## 🛠 Configuração no VS Code (Opcional)

Include Path recomendado:

```
${workspaceFolder}/**
C:\msys64\mingw64\include
```

Compiler Path:

```
C:\msys64\mingw64\bin\g++.exe
```

---

## 👨‍💻 Autores

Fabricio Bastos Cardoso
Davi Oliveira
-
-
-

