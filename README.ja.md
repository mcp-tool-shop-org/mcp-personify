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

**プロトタイプ** - Unreal Engine 5上で動作する、VRMアバター。リアルタイムのテキスト読み上げ（TTS）による口の動きの同期と、表情の反映機能を搭載しています。

リアルタイムで動作するVRMアバターシステムで、ローカルのTTS（テキスト読み上げ）ブリッジと連携し、音声合成、口の動きの同期（ビゼーム駆動）、および感情表現の対応を実現します。Godotのプロトタイプ([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp))をベースに移植されました。

## 状態

| 特徴 | State |
| ご依頼ありがとうございます。翻訳を開始します。 | The company is committed to providing high-quality products and services.
(会社は、高品質な製品とサービスを提供することに尽力しています。) |
| VRM 0.x / 1.0形式のデータ読み込み (VRM4U) | 足場 |
| TTS連携用WebSocketブリッジ。 | 足場 |
| 唇の動きの同期（5段階）。 | 足場 |
| 感情表現 | 足場 |
| ブリッジ機能の自動起動。 | 足場 |
| モーフターゲットキャッシュ | 足場 |

## スタック

| 部品
構成要素
構成
構成品 | 選択肢。
または、選択。 |
| 申し訳ありませんが、翻訳するテキストが提供されていません。テキストを入力してください。 | 申し訳ありませんが、翻訳するテキストが提供されていません。テキストを入力してください。 |
| エンジン。 | Unreal Engine 5.7.3 |
| VRM | [VRM4U](https://github.com/ruyo/VRM4U) プラグイン。 |
| WebSocket | 組み込みの `IWebSocket` (WebSocket モジュール) |
| TTS | [voice-soundboard-mcp] (https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) を、ブリッジ経由で利用します。 |
| Audio | `UAudioComponent` と、振幅に基づいた視覚的な表現（ビゼーム）。 |

## セットアップ

### 前提条件
- Unreal Engine 5.7 (長期サポート版)
- Node.js 18 以降
- `@mcptoolshop/voice-soundboard-mcp` をグローバルにインストール済み

### 1. クローンを作成して開く
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
UE5で`mcppersonify.uproject`ファイルを開きます。すると、C++モジュールのコンパイルを促すメッセージが表示されます。

### 2. VRM4Uプラグインをインストールする
1. [VRM4U リリース](https://github.com/ruyo/VRM4U/releases) から `VRM4U_5_7.zip` をダウンロードしてください。
2. `VRM4U/` フォルダを `Plugins/` ディレクトリに展開してください。
3. エディタを再起動し、Edit > Plugins でプラグインを有効にしてください。

### 3. ブリッジに必要な依存関係をインストールする
```bash
cd tools/tts-bridge
npm install
```

### 4. VRMアバターを追加する
`.vrm`ファイルを`Content/Avatars/`フォルダに配置してください。ローダーは自動的に最初のファイルを見つけます。

### 5. シーンを設定する
1. レベル内に「PersonifyManager」というアクターを配置します（アクター配置パネルで検索してください）。
2. プレイボタンを押すと、自動的にブリッジが起動し、アバターが読み込まれ、テキスト読み上げ（TTS）の準備が完了します。

## アーキテクチャ

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

## ソースファイル

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

## ブリッジプロトコル

**Unreal -> Bridge:**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — 利用可能な音声を取得します。
- `{ "type": "stop" }` — 処理を中断します。

**Bridge -> Unreal:**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## v0.1.0 へのロードマップ

- [ ] C++モジュールがUE 5.7でエラーなくコンパイルされる。
- [ ] VRM4Uがアバターを読み込み、シーンに表示する。
- [ ] ブリッジが接続され、TTS音声を受信する。
- [ ] 表情アニメーションが口の形状を制御する。
- [ ] 表情の表現が徐々に変化する（オン/オフ）。
- [ ] ブリッジが再生時に自動的に起動する。
- [ ] ワンクリックでパッケージングされた(.exe)ファイルが、最初から最後まで正常に動作する。

## ライセンス

マサチューセッツ工科大学
