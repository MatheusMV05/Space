![bannergithub](https://imgur.com/861U3NS.png)

<p align="center"><b>Invasores do espaço</b> é um clone do famoso jogo Space Invaders desenvolvido em C utilizando a cli-lib e raylib.</p>

---
## Em sistemas Linux:
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
cd Space
```

7º)  Compile e rode o jogo:
```
make run
```
Para mais informações, consulte a [documentação oficial da Raylib para Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux).

<br>

## Em sistemas macOS:

1º) Configure o ambiente para suportar a compilação estática:
```
export MACOSX_DEPLOYMENT_TARGET=10.9
```

2º) Instale as ferramentas XCode (certifique-se de atualizar as ferramentas após a instalação):
```
xcode-select --install
```

3º) Baixe o código-fonte do Raylib diretamente do GitHub e entre na pasta:
```
git clone https://github.com/raysan5/raylib.git
cd raylib/src
```

4º) Compile a raylib para garantir que a configuração estática esteja aplicada:
```
make
```

5º) Clone o repositório do jogo:
```
git clone https://github.com/MatheusMV05/Space.git
cd Space
```

6º) Compile e rode o jogo usando o seguinte comando:
```
make run
```

Para mais informações, consulte a [documentação oficial da Raylib para macOS](https://github.com/raysan5/raylib/wiki/Working-on-macOS).

---
