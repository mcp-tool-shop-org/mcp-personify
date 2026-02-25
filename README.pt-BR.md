<p align="center">
  <strong>English</strong> | <a href="README.ja.md">日本語</a> | <a href="README.zh.md">中文</a> | <a href="README.es.md">Español</a> | <a href="README.fr.md">Français</a> | <a href="README.hi.md">हिन्दी</a> | <a href="README.it.md">Italiano</a> | <a href="README.pt-BR.md">Português</a>
</p>

<p align="center">
  <img src="assets/logo.png" alt="MCP Personify" width="400" />
</p>

<p align="center">
  <a href="https://github.com/mcp-tool-shop-org/mcp-personify/actions/workflows/ci.yml"><img src="https://github.com/mcp-tool-shop-org/mcp-personify/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue" alt="MIT License"></a>
  <a href="https://mcp-tool-shop-org.github.io/mcp-personify/"><img src="https://img.shields.io/badge/Landing_Page-live-blue" alt="Landing Page"></a>
</p>

**Protótipo** — Avatar VRM com sincronização labial em tempo real e mapeamento de expressões, desenvolvido na Unreal Engine 5.

Sistema de avatares VRM em tempo real que se conecta a uma ponte local de síntese de voz para gerar fala, sincronização labial baseada em visemas e mapeamento de expressões emocionais. Adaptado do protótipo do Godot ([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp)).

## Status

| Característica. | State |
| Please provide the English text you would like me to translate. I am ready to translate it into Portuguese. | Please provide the English text you would like me to translate. I am ready to translate it into Portuguese. |
| Carregamento da versão 0.x / 1.0 do VRM (VRM4U). | Andaime. |
| Ponte TTS (WebSocket). | Andaime. |
| Sincronização labial de visemes (5 bandas). | Andaime. |
| Expressões de emoção. | Andaime. |
| Partida automática do sistema de ponte. | Andaime. |
| Cache de modelos de deformação. | Andaime. |

## Pilha

| Componente. | Escolha. |
| Por favor, forneça o texto em inglês que você gostaria que eu traduzisse para o português. | Please provide the English text you would like me to translate. I am ready to translate it into Portuguese. |
| Motor. | Unreal Engine 5.7.3 |
| VRM | Plugin [VRM4U](https://github.com/ruyo/VRM4U) |
| WebSocket | Módulo `IWebSocket` (WebSockets) integrado. |
| TTS | [painel de sons de voz - mcp](https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) via bridge. |
| Audio | `UAudioComponent` + visemas baseados na amplitude. |

## Configuração

### Pré-requisitos
- Unreal Engine 5.7 (versão de suporte de longo prazo)
- Node.js 18 ou superior
- Pacote `@mcptoolshop/voice-soundboard-mcp` instalado globalmente.

### 1. Clonar e abrir
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
Abra o arquivo `mcppersonify.uproject` no UE5. O programa solicitará a compilação do módulo C++.

### 2. Instale o plugin VRM4U
1. Faça o download do arquivo `VRM4U_5_7.zip` em [lançamentos do VRM4U](https://github.com/ruyo/VRM4U/releases).
2. Extraia a pasta `VRM4U/` para o diretório `Plugins/`.
3. Reinicie o editor e ative o plugin em Editar > Plugins.

### 3. Instale as dependências da ponte
```bash
cd tools/tts-bridge
npm install
```

### 4. Adicione um avatar VRM
Coloque qualquer arquivo `.vrm` na pasta `Content/Avatars/`. O carregador detectará automaticamente o primeiro arquivo.

### 5. Prepare o cenário
1. Coloque um ator `PersonifyManager` no seu nível (procure no painel "Colocar Atores").
2. Clique em "Reproduzir" — a ponte será iniciada automaticamente, o avatar será carregado e estará pronto para a síntese de voz.

## Arquitetura

```
Claude Code / MCP Host
    |
    v (MCP stdio)
voice-soundboard-mcp ──> .ogg/.wav files
    ^
    | (MCP stdio)
mcp-aside (optional) ──> emotion cues
    ^
    |
tools/tts-bridge/bridge.mjs  (Node.js, WebSocket on :9200)
    ^
    | (WebSocket JSON)
    v
Unreal Engine (UBridgeClient)
    |
    ├── UTtsAudioPlayer ──> UAudioComponent
    |       |
    |       v
    |   UVisemeDriverFFT ──> 5 viseme bands
    |       |
    |       v
    ├── UExpressionMapper ──> morph target weights
    |       |
    |       v
    └── UVrmAvatarLoader ──> VRM4U runtime load
            |
            v
        FBlendShapeCache ──> USkeletalMeshComponent lookup
```

## Arquivos de origem

```
Source/mcppersonify/
  mcppersonify.h/.cpp          Module entry point
  mcppersonify.Build.cs        Module build config
  PersonifyManager.h/.cpp      Main orchestrator actor
  BridgeClient.h/.cpp          WebSocket client (IWebSocket)
  TtsAudioPlayer.h/.cpp        Audio file loading + playback
  VisemeDriverFFT.h/.cpp       Amplitude → viseme bands
  BlendShapeCache.h/.cpp       Morph target name lookup
  ExpressionMapper.h/.cpp      Viseme/emotion → morph targets
  VrmAvatarLoader.h/.cpp       VRM4U runtime loader

tools/tts-bridge/
  bridge.mjs                   Node.js WebSocket ↔ MCP bridge
  package.json
```

## Protocolo de Ponte

**Unreal -> Bridge:**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — obter as vozes disponíveis
- `{ "type": "stop" }` — interromper

**Ponte -> Irreal:**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## Roteiro para a versão 0.1.0

- [ ] O módulo C++ compila sem erros no UE 5.7.
- [ ] O VRM4U carrega e renderiza o avatar na cena.
- [ ] A ponte estabelece a conexão e recebe o áudio de síntese de voz (TTS).
- [ ] As expressões faciais controlam as deformações da boca.
- [ ] As expressões emocionais são ativadas e desativadas gradualmente.
- [ ] A ponte inicia automaticamente ao iniciar o jogo.
- [ ] O pacote de instalação de um clique (.exe) funciona corretamente do início ao fim.

## Licença

MIT.
