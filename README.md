# CHIP-8 Emulator

Emulador do CHIP-8 desenvolvido em C++17, com SDL2 para a janela, entrada de teclado, renderização e áudio. O projeto usa CMake para organizar a compilação e foi estruturado para ser compilado em diferentes sistemas operacionais.

O projeto está em desenvolvimento; a compatibilidade com ROMs depende das instruções e dos comportamentos já implementados.

## Componentes

A implementação separa as responsabilidades em classes:

| Componente | Responsabilidade |
|---|---|
| `Memory` | Memória de 4 KiB e carregamento do programa e dos sprites de fonte. |
| `CPU` | Registradores, contador de programa, pilha e execução das instruções. |
| `Decoder` | Decodificação e encaminhamento dos opcodes para suas famílias de instruções. |
| `Renderer` | Framebuffer monocromático de 64 × 32 pixels e desenho de sprites. |
| `Input` | Estado das 16 teclas CHIP-8 e registro de novos pressionamentos. |
| `Sound` | Estado e frequência lógica do som. |
| `Temporizer` | Controle do tempo decorrido, da taxa de instruções e dos temporizadores de 60 Hz. |
| `Interface` | Janela SDL2, eventos do teclado, renderização da imagem e reprodução de áudio. |
| `Emulator` | Criação e conexão dos componentes, carregamento da ROM e loop principal. |

## Funcionalidades

- Memória CHIP-8 de 4096 bytes; os programas são carregados a partir do endereço `0x200`.
- Registradores da CPU, pilha de chamadas e temporizadores de atraso e som.
- Decodificação de instruções organizada por famílias, incluindo fluxo de execução, transferência de dados, lógica, comparação, gráficos, entrada e temporizadores.
- Tela monocromática de 64 × 32 pixels, com desenho de sprites e detecção de colisão.
- Entrada por teclado para as 16 teclas CHIP-8.
- Áudio básico gerado com SDL2.
- Acumuladores de tempo para separar a execução das instruções dos temporizadores de 60 Hz.

## Estrutura do projeto

```text
.
├── CMakeLists.txt
├── run.bat
├── run.bash
├── include/
│   ├── cpu.hpp
│   ├── decoder.hpp
│   ├── emulator.hpp
│   ├── input.hpp
│   ├── interface.hpp
│   ├── memory.hpp
│   ├── renderer.hpp
│   ├── sound.hpp
│   └── temporizer.hpp
├── src/
│   └── *.cpp
├── roms/                 # Opcional: ROMs usadas localmente
└── build/                # Gerada pelo CMake
```

O CMake procura automaticamente os arquivos `.cpp` em `src/`. Não é necessário manter uma lista manual de cada novo arquivo-fonte.

## Dependências

Você precisa de:

- CMake 3.20 ou superior.
- Um compilador compatível com C++17.
- SDL2 e os respectivos arquivos de desenvolvimento.

Os scripts não instalam as dependências por você.

### Windows

Instale o CMake, um compilador C++ (por exemplo, MSVC, GCC/MinGW ou Clang) e a SDL2 compatível com esse compilador. Execute os comandos a partir de um terminal no qual o CMake e o compilador escolhido estejam disponíveis.

Se estiver usando MSYS2 UCRT64, instale as ferramentas e a SDL2 pelo ambiente UCRT64 e execute o script em um terminal configurado para esse ambiente.

### Linux

Em distribuições Debian ou Ubuntu, as dependências podem ser instaladas com:

```bash
sudo apt update
sudo apt install cmake build-essential libsdl2-dev pkg-config
```

Em outras distribuições, instale os pacotes equivalentes usando o gerenciador de pacotes do sistema.

## Como compilar e rodar

Os scripts configuram o projeto com CMake e compilam o executável. Se você fornecer o caminho de uma ROM como argumento, também tentam executar o emulador com essa ROM.

### Windows

Para configurar e compilar:

```bat
run.bat
```

Para compilar e abrir uma ROM:

```bat
run.bat "roms\sua-rom.ch8"
```

O executável será gerado em:

```text
build\bin\Release\chip8.exe
```

### Linux

Dê permissão de execução ao script:

```bash
chmod +x run.bash
```

Para configurar e compilar:

```bash
./run.bash
```

Para compilar e abrir uma ROM:

```bash
./run.bash "roms/sua-rom.ch8"
```

O executável será gerado em:

```text
build/bin/Release/chip8
```

Substitua `roms/sua-rom.ch8` pelo caminho real da ROM. Você também pode informar um caminho absoluto. O argumento será encaminhado ao executável; o `main.cpp` deve repassá-lo ao método `Emulator::start()`.

## Compilador e builds

O CMake não fixa GCC, Clang ou MSVC no `CMakeLists.txt`: usa o gerador e o compilador disponíveis no ambiente de configuração. A escolha fica registrada em `build/`. Se quiser trocar de compilador ou de gerador, remova a pasta `build/` e execute o script novamente em um ambiente configurado para o compilador desejado.

O executável é nativo do sistema em que foi compilado: o Windows gera `chip8.exe`, enquanto o Linux gera `chip8`. Para usar o projeto em outro sistema operacional, compile o mesmo código-fonte nesse sistema.

## Solução de problemas

- **SDL2 não encontrada:** instale os arquivos de desenvolvimento da SDL2 compatíveis com o compilador usado. Se necessário, indique a instalação ao CMake com `CMAKE_PREFIX_PATH`.
- **Nenhum compilador encontrado:** instale um compilador C++17 e execute o script em um terminal que tenha o compilador no ambiente.
- **Mudou de compilador ou gerador:** exclua `build/` antes de configurar novamente.
- **O programa não inicia uma ROM:** confira o caminho informado e se o `main.cpp` recebe o argumento e chama `Emulator::start()` com ele.
