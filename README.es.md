<p align="center">
  <strong>English</strong> | <a href="README.ja.md">日本語</a> | <a href="README.zh.md">中文</a> | <a href="README.es.md">Español</a> | <a href="README.fr.md">Français</a> | <a href="README.hi.md">हिन्दी</a> | <a href="README.it.md">Italiano</a> | <a href="README.pt-BR.md">Português</a>
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/mcp-tool-shop-org/brand/main/logos/mcp-personify/readme.png" alt="MCP Personify" width="400" />
</p>

<p align="center">
  <a href="https://github.com/mcp-tool-shop-org/mcp-personify/actions/workflows/ci.yml"><img src="https://github.com/mcp-tool-shop-org/mcp-personify/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue" alt="MIT License"></a>
  <a href="https://mcp-tool-shop-org.github.io/mcp-personify/"><img src="https://img.shields.io/badge/Landing_Page-live-blue" alt="Landing Page"></a>
</p>

**Prototipo:** Avatar VRM con sincronización labial en tiempo real mediante tecnología de síntesis de voz y mapeo de expresiones, implementado en Unreal Engine 5.

Sistema de avatares VRM en tiempo real que se conecta a un puente local de síntesis de voz para la generación de habla, la sincronización labial basada en visemas y la representación de emociones. Adaptado del prototipo de Godot ([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp)).

## Estado

| Característica. | State |
| Please provide the English text you would like me to translate. I am ready to translate it into Spanish. | Please provide the English text you would like me to translate. I am ready to translate it into Spanish. |
| Carga de VRM 0.x / 1.0 (VRM4U). | Andamio. |
| Puente TTS (WebSocket). | Andamio. |
| Sincronización labial (5 bandas). | Andamio. |
| Expresiones de emociones. | Andamio. |
| Arranque automático del puente. | Andamio. |
| Caché de modelos de deformación. | Andamio. |

## Pila

| Componente. | Elección. |
| Por favor, proporcione el texto que desea que traduzca. | "Please provide the text you would like me to translate." |
| Motor. | Unreal Engine 5.7.3. |
| VRM | Plugin [VRM4U](https://github.com/ruyo/VRM4U) |
| WebSocket. | Módulo `IWebSocket` integrado (módulo de WebSockets). |
| TTS | [voice-soundboard-mcp](https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) a través de un puente. |
| Audio | Componente de audio UAudio + visemas basados en la amplitud. |

## Configuración

### Requisitos previos
- Unreal Engine 5.7 (versión de soporte a largo plazo)
- Node.js 18 o superior
- `@mcptoolshop/voice-soundboard-mcp` instalado a nivel global.

### 1. Clonar y abrir
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
Abra el archivo `mcppersonify.uproject` en UE5. Esto le solicitará que compile el módulo en C++.

### 2. Instale el plugin VRM4U
1. Descargue el archivo `VRM4U_5_7.zip` desde [las versiones de VRM4U](https://github.com/ruyo/VRM4U/releases).
2. Extraiga la carpeta `VRM4U/` en el directorio `Plugins/`.
3. Reinicie el editor y active el complemento en Editar > Complementos.

### 3. Instale las dependencias del puente
```bash
cd tools/tts-bridge
npm install
```

### 4. Añadir un avatar VRM
Coloque cualquier archivo `.vrm` en la carpeta `Content/Avatars/`. El cargador detectará automáticamente el primer archivo.

### 5. Preparar el escenario
1. Coloque un actor de tipo `PersonifyManager` en su nivel (busque en el panel "Colocar Actores").
2. Presione "Reproducir" — el puente se iniciará automáticamente, el avatar se cargará y estará listo para la síntesis de voz.

## Arquitectura

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

## Archivos de origen

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

## Protocolo de puente

**Unreal -> Puente:**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — obtener las voces disponibles
- `{ "type": "stop" }` — interrumpir

**Puente -> Irreal:**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## Hoja de ruta para la versión 0.1.0

- [ ] El módulo de C++ se compila correctamente en UE 5.7.
- [ ] VRM4U carga y renderiza el avatar en la escena.
- [ ] El puente se conecta y recibe el audio de síntesis de voz (TTS).
- [ ] Los visemas controlan las deformaciones de la boca.
- [ ] Las expresiones emocionales se activan y desactivan.
- [ ] El puente se inicia automáticamente al reproducir.
- [ ] El paquete de instalación de un solo clic (.exe) funciona de principio a fin.

## Licencia

MIT.
