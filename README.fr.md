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

**Prototype** — Avatar VRM avec synchronisation labiale en temps réel et correspondance des expressions, intégré à Unreal Engine 5.

Système d'avatar VRM fonctionnant en temps réel, qui se connecte à une passerelle TTS locale pour la synthèse vocale, la synchronisation labiale basée sur les visèmes et la représentation des émotions. Porté du prototype Godot ([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp)).

## Statut

| Fonctionnalité. | State |
| Veuillez fournir le texte à traduire. | Please provide the English text you would like me to translate. I am ready to translate it into French. |
| Chargement de la version 0.x / 1.0 de VRM (VRM4U). | échafaudage |
| Pont TTS (WebSocket). | échafaudage |
| Synchronisation labiale (5 bandes). | échafaudage |
| Expressions émotionnelles. | échafaudage |
| Démarrage automatique du pont. | échafaudage |
| Cache des modèles morphologiques. | échafaudage |

## Pile

| Composant. | Choix. |
| Bien sûr, veuillez me fournir le texte que vous souhaitez que je traduise. | Veuillez fournir le texte à traduire. |
| Moteur. | Unreal Engine 5.7.3. |
| VRM | [VRM4U](https://github.com/ruyo/VRM4U) : plugin. |
| WebSocket. | Module `IWebSocket` intégré (module WebSockets). |
| TTS | [voice-soundboard-mcp](https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) via pont. |
| Audio | `UAudioComponent` + visèmes basés sur l'amplitude. |

## Installation

### Prérequis
- Unreal Engine 5.7 (version LTS)
- Node.js 18 ou version ultérieure
- Le paquet `@mcptoolshop/voice-soundboard-mcp` installé globalement.

### 1. Dupliquer et ouvrir
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
Ouvrez le fichier `mcppersonify.uproject` dans UE5. Vous serez invité à compiler le module C++.

### 2. Installez le plugin VRM4U
1. Téléchargez le fichier `VRM4U_5_7.zip` depuis la page des versions de [VRM4U](https://github.com/ruyo/VRM4U/releases).
2. Extrayez le dossier `VRM4U/` dans le répertoire `Plugins/`.
3. Redémarrez l'éditeur, puis activez le plugin dans le menu Édition > Plugins.

### 3. Installer les dépendances du pont
```bash
cd tools/tts-bridge
npm install
```

### 4. Ajoutez un avatar VRM
Placez tout fichier `.vrm` dans le dossier `Content/Avatars/`. Le système de chargement détecte automatiquement le premier fichier.

### 5. Mettez en scène
1. Placez un acteur `PersonifyManager` dans votre niveau (recherchez-le dans le panneau "Place Actors").
2. Appuyez sur "Lecture" : le pont se lance automatiquement, l'avatar se charge et est prêt pour la synthèse vocale.

## Architecture

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

## Fichiers sources

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

## Protocole de pont

**Unreal -> Bridge :**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — obtenir la liste des voix disponibles
- `{ "type": "stop" }` — interrompre

**Pont -> Irrélisé :**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## Feuille de route vers la version 0.1.0

- [ ] Le module C++ se compile correctement dans UE 5.7.
- [ ] VRM4U charge et affiche l'avatar dans la scène.
- [ ] Le pont se connecte et reçoit l'audio de synthèse vocale.
- [ ] Les visèmes contrôlent les déformations de la bouche.
- [ ] Les expressions émotionnelles s'activent et se désactivent progressivement.
- [ ] Le pont démarre automatiquement au lancement.
- [ ] Le package unique (.exe) fonctionne de bout en bout en un seul clic.

## Licence

MIT.
