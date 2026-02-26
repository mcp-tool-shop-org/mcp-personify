<p align="center">
  <strong>English</strong> | <a href="README.ja.md">日本語</a> | <a href="README.zh.md">中文</a> | <a href="README.es.md">Español</a> | <a href="README.fr.md">Français</a> | <a href="README.hi.md">हिन्दी</a> | <a href="README.it.md">Italiano</a> | <a href="README.pt-BR.md">Português</a>
</p>

<p align="center">
  
            <img src="https://raw.githubusercontent.com/mcp-tool-shop-org/brand/main/logos/mcp-personify/readme.png"
           alt="MCP Personify" width="400" />
</p>

<p align="center">
  <a href="https://github.com/mcp-tool-shop-org/mcp-personify/actions/workflows/ci.yml"><img src="https://github.com/mcp-tool-shop-org/mcp-personify/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue" alt="MIT License"></a>
  <a href="https://mcp-tool-shop-org.github.io/mcp-personify/"><img src="https://img.shields.io/badge/Landing_Page-live-blue" alt="Landing Page"></a>
</p>

**Prototipo** – Avatar VRM con sincronizzazione labiale in tempo reale tramite sintesi vocale e mappatura delle espressioni, realizzato con Unreal Engine 5.

Sistema di avatar VRM in esecuzione che si connette a un bridge TTS locale per la sintesi vocale, la sincronizzazione labiale basata su visemi e la mappatura delle espressioni emotive. Adattato dal prototipo Godot ([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp)).

## Stato

| Caratteristica. | State |
| Certo, ecco la traduzione:

"Please provide the English text you would like me to translate into Italian." | Certainly. Please provide the English text you would like me to translate. |
| Caricamento di VRM 0.x / 1.0 (VRM4U). | Impalcatura. |
| Ponte TTS (WebSocket). | Impalcatura. |
| Sincronizzazione labiale (5 fasce). | Impalcatura. |
| Espressioni emotive. | Impalcatura. |
| Avvio automatico del ponte. | Impalcatura. |
| Cache delle forme deformabili. | Impalcatura. |

## Pila

| Componente. | Scelta. |
| Certo, ecco la traduzione:

"Please provide the English text you would like me to translate into Italian." | Certo, ecco la traduzione:

"Please provide the English text you would like me to translate into Italian." |
| Motore. | Unreal Engine 5.7.3. |
| VRM | Plugin [VRM4U](https://github.com/ruyo/VRM4U). |
| WebSocket. | Modulo `IWebSocket` integrato (modulo per WebSockets). |
| TTS | [voice-soundboard-mcp](https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) tramite connessione di rete. |
| Audio | `UAudioComponent` + visemi basati sull'ampiezza. |

## Configurazione

### Prerequisiti
- Unreal Engine 5.7 (versione LTS)
- Node.js versione 18 o superiore
- Pacchetto `@mcptoolshop/voice-soundboard-mcp` installato a livello globale.

### 1. Duplicare e aprire
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
Aprire il file `mcppersonify.uproject` in UE5. Verrà richiesto di compilare il modulo C++.

### 2. Installare il plugin VRM4U
1. Scaricare il file `VRM4U_5_7.zip` dalla sezione [rilasci di VRM4U](https://github.com/ruyo/VRM4U/releases).
2. Estrarre la cartella `VRM4U/` nella directory `Plugins/`.
3. Riavviare l'editor e attivare il plugin in "Modifica > Plugin".

### 3. Installare le dipendenze del bridge
```bash
cd tools/tts-bridge
npm install
```

### 4. Aggiungere un avatar VRM
Inserire qualsiasi file con estensione `.vrm` nella cartella `Content/Avatars/`. Il programma di caricamento rileva automaticamente il primo file.

### 5. Preparare l'ambientazione
1. Inserire un attore di tipo `PersonifyManager` nel livello (cercare nel pannello "Inserisci attori").
2. Premere "Avvia" – il sistema di sintesi vocale si avvierà automaticamente, l'avatar verrà caricato e sarà pronto per l'utilizzo.

## Architettura

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

## File sorgente

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

## Protocollo Bridge

**Unreal -> Bridge:**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — ottenere le voci disponibili
- `{ "type": "stop" }` — interrompere

**Bridge -> Unreal:**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## Roadmap per la versione 0.1.0

- [ ] Il modulo C++ compila correttamente in UE 5.7.
- [ ] VRM4U carica e visualizza l'avatar nella scena.
- [ ] Il bridge si connette e riceve l'audio del text-to-speech.
- [ ] Le viseme controllano le deformazioni della bocca.
- [ ] Le espressioni emotive si attivano e disattivano gradualmente.
- [ ] Il bridge si avvia automaticamente all'avvio del programma.
- [ ] Il pacchetto con un solo clic (.exe) funziona correttamente dall'inizio alla fine.

## Licenza

MIT.
