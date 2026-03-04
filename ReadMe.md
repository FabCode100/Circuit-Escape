# 🔌 Circuit Escape: Phase 1

<p align="center">
  <img src="src/assets/prof_leo.png" width="128" alt="Professor Leo">
  <br>
  <i>"A eletricidade é a alma do mundo, e os circuitos são seu esqueleto!" — Prof. Leo</i>
</p>

---

## 🎮 O Jogo

**Circuit Escape** é um RPG de aventura educacional focado em STEM (Ciência, Tecnologia, Engenharia e Matemática). Você assume o papel de **Berta**, uma jovem aspirante a engenheira que deve explorar um laboratório tecnológico, resolver puzzles de circuitos reais e escapar de armadilhas elétricas.

### ✨ Destaques da Fase 1
* **Exploração 2D**: Navegue por um laboratório em pixel art com câmera dinâmica e colisão.
* **Sistema de Diálogo**: Interaja com o **Professor Leo**, um cientista entusiasta que te guiará através de mensagens estilizadas e retratos expressivos.
* **Mecânica de Puzzle**: Monte circuitos reais (Bateria + Resistores + LEDs) em uma interface dedicada.
* **Sistema de VFX & Áudio**: Efeitos de fumaça ao queimar componentes, screen shake em falhas e feedback sonoro imersivo.

---

## 🚀 Tecnologias
* **C++17**: Lógica de jogo e sistemas core.
* **SDL2**: Renderização acelerada por hardware.
* **SDL2_image**: Suporte para texturas PNG.
* **SDL2_ttf**: Renderização de fontes (Press Start 2P).
* **SDL2_mixer**: Sistema de áudio multicanal.

---

## 📦 Configuração e Instalação

### 1. Pré-requisitos (MSYS2 MinGW64)
Abra o seu terminal **MSYS2 MinGW64** e instale as dependências:

```bash
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-SDL2 \
          mingw-w64-x86_64-SDL2_image \
          mingw-w64-x86_64-SDL2_ttf \
          mingw-w64-x86_64-SDL2_mixer
```

### 2. Compilação
Vá até a pasta `src/` do projeto e execute:

```bash
g++ main.cpp Game.cpp Player.cpp TileMap.cpp Tile.cpp Camera.cpp CircuitSystem.cpp CircuitPuzzle.cpp DialogueSystem.cpp VFX.cpp AudioSystem.cpp HardwareInterface.cpp AchievementSystem.cpp -o game.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -mwindows && ./game.exe
```

---

## ⌨️ Controles

| Tecla | Ação |
|-------|------|
| **Setas** | Movimentação / Navegação no Menu |
| **Espaço / E** | Interação / Próximo Diálogo |
| **Tab** | Trocar componente no Puzzle |
| **R** | Rotacionar componente |
| **V** | Validar Circuito |
| **Esc** | Sair do Puzzle / Menu |

---

## � Mecânicas de Circuito
Nesta fase, você aprende o conceito básico de **proteção de componentes**:
1. **Fonte (BAT)**: Provê energia para o loop.
2. **Resistor (RES)**: OBRIGATÓRIO para limitar a corrente.
3. **LED**: Componente de saída. Se ligado diretamente na bateria... **CUIDADO COM A FUMAÇA!** 💨

---

## � Assets
* **Fonte**: [Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P)
* **Retrato**: Professor Leo (Cientista Maluco)
* **Sprites**: Componentes Eletrônicos em Pixel Art 32x32

---

## 👨‍💻 Autor
* **Fabricio Bastos Cardoso**

---
<p align="center">Desenvolvido com ❤️ para ensinar engenharia através da diversão.</p>
