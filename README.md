![bannergithub](https://imgur.com/Zim6bs3.png)

<p align="center"><b>Invasores do espaço</b> é um jogo de pesca desenvolvido em C utilizando a raylib, baseado no famoso jogo space invaders.</p>

---

# ⚙️ Como rodar:

1º)  Você precisará do GCC, make e git para baixar e compilar a biblioteca:
```
sudo apt install build-essential git
```

2º)  Em seguida, instale as bibliotecas necessárias para áudio, gráficos OpenGL e sistema de janelas X11:
```
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

3º)  Baixe o código-fonte do Raylib diretamente do GitHub e entre na pasta:
```
git clone --depth 1 https://github.com/raysan5/raylib.git raylib
cd raylib/src/
```

4º)  Compile a raylib:
```
make PLATFORM=PLATFORM_DESKTOP
```

5º)  Instale a raylib:
```
sudo make install
cd ~
```

6º)  Clone o repositório do jogo:
```
git clone https://github.com/MatheusMV05/Space.git
cd src
```

7º)  Compile e rode o jogo:
```
make run
```

<br>
