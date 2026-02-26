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

**原型：** 在虚幻引擎5中，一个具有实时语音合成（TTS）口型同步和表情映射功能的VRM虚拟人物模型。

这是一个实时VRM虚拟人物系统，它通过本地TTS（文本转语音）桥接，实现语音合成、唇形同步以及情感表达映射。该系统是基于Godot引擎的原型项目([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp))的移植版本。

## 状态

| 特点。 | State |
| 好的，请提供需要翻译的英文文本。 | 好的，请提供需要翻译的英文文本。 |
| VRM 0.x / 1.0 版本加载 (VRM4U)。 | 脚手架 |
| TTS 桥接 (WebSocket) | 脚手架 |
| 唇部同步动画（5个档位）。 | 脚手架 |
| 情感表达。 | 脚手架。 |
| 桥接自动启动。 | 脚手架。 |
| 形状模型缓存。 | 脚手架 |

## 堆栈

| 组件。 | 选择。 |
| 好的，请提供需要翻译的英文文本。 | 好的，请提供需要翻译的英文文本。 |
| 发动机。 | Unreal Engine 5.7.3 |
| VRM | [VRM4U](https://github.com/ruyo/VRM4U) 插件。 |
| WebSocket。 | 内置的 `IWebSocket` (WebSocket 模块)。 |
| TTS | [voice-soundboard-mcp](https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) (通过桥接方式) |
| Audio | `UAudioComponent` 组件 + 基于振幅的视觉音素。 |

## 设置

### 先决条件
- Unreal Engine 5.7 LTS版本
- Node.js 18.0及以上版本
- `@mcptoolshop/voice-soundboard-mcp` 已全局安装。

### 1. 克隆并打开
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
在UE5中打开 `mcppersonify.uproject` 文件。系统会提示您编译C++模块。

### 2. 安装 VRM4U 插件
1. 从 [VRM4U 发布页面](https://github.com/ruyo/VRM4U/releases) 下载 `VRM4U_5_7.zip` 文件。
2. 将 `VRM4U/` 文件夹解压到 `Plugins/` 目录下。
3. 重新启动编辑器，然后在“编辑”>“插件”中启用该插件。

### 3. 安装桥接功能的依赖项
```bash
cd tools/tts-bridge
npm install
```

### 4. 添加一个VRM虚拟人物
将任何 `.vrm` 文件放置在 `Content/Avatars/` 目录下。加载器会自动检测并加载其中的第一个文件。

### 5. 搭建场景
1. 在您的关卡中放置一个 `PersonifyManager` 角色（在“放置角色”面板中搜索）。
2. 点击“开始”按钮 — 桥接功能将自动启动，角色模型将加载，准备好进行文本转语音（TTS）功能。

## 架构

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

## 源文件

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

## 桥接协议

**Unreal -> Bridge：**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — 获取可用的语音选项
- `{ "type": "stop" }` — 停止当前操作

**桥接 -> 模拟：**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## v0.1.0版本的开发路线图

- [ ] C++ 模块在 UE 5.7 中编译通过。
- [ ] VRM4U 加载并渲染场景中的角色。
- [ ] Bridge 连接并接收文本转语音的音频。
- [ ] 口型动画驱动嘴部形变。
- [ ] 情绪表情可以逐渐显示/隐藏。
- [ ] Bridge 在游戏开始时自动启动。
- [ ] 一键打包 (.exe) 完整流程正常工作。

## 许可

麻省理工学院。
