import type { SiteConfig } from '@mcptoolshop/site-theme';

export const config: SiteConfig = {
  title: 'MCP Personify',
  description: 'Prototype VRM avatar with real-time TTS lipsync and expression mapping — Unreal Engine 5, VRM4U, WebSocket bridge',
  logoBadge: 'P',
  brandName: 'MCP Personify',
  repoUrl: 'https://github.com/mcp-tool-shop-org/mcp-personify',
  footerText: 'MIT Licensed — built by <a href="https://github.com/mcp-tool-shop-org" style="color:var(--color-muted);text-decoration:underline">mcp-tool-shop-org</a>',

  hero: {
    badge: 'Prototype · UE5',
    headline: 'Your AI agent,',
    headlineAccent: 'with a face.',
    description: 'VRM avatar with real-time TTS lipsync and expression mapping in Unreal Engine 5. Speaks through voice-soundboard-mcp, drives visemes from audio amplitude, maps emotions to morph targets.',
    primaryCta: { href: 'https://github.com/mcp-tool-shop-org/mcp-personify', label: 'View on GitHub' },
    secondaryCta: { href: '#architecture', label: 'See the architecture' },
    previews: [
      { label: 'Speak', code: '{ "type": "speak", "text": "Hello", "voice": "am_fenrir",\n  "emotion": "happy", "intensity": 0.7 }' },
      { label: 'Response', code: '{ "type": "tts.play", "path": "...", "durationMs": 1200,\n  "emotion": "happy", "intensity": 0.7 }' },
      { label: 'Lipsync', code: 'UAudioComponent → UVisemeDriverFFT\n  → 5 amplitude bands\n  → morph target weights' },
    ],
  },

  sections: [
    {
      kind: 'features',
      id: 'features',
      title: 'What It Does',
      subtitle: 'Real-time avatar system that bridges MCP tool output to a living, speaking UE5 character.',
      features: [
        {
          title: 'Real-Time Lipsync',
          desc: 'Audio amplitude feeds a 5-band viseme driver (UVisemeDriverFFT). Morph targets update every frame — no pre-baked clips, no manual phoneme alignment.',
        },
        {
          title: 'Emotion Expressions',
          desc: 'Emotion cues from the bridge (happy, sad, angry, neutral…) ramp morph target weights through UExpressionMapper. Intensity is a float — blend as needed.',
        },
        {
          title: 'WebSocket Bridge',
          desc: 'A Node.js bridge (tools/tts-bridge) sits between voice-soundboard-mcp and UE5. UBridgeClient connects over WebSocket — no engine restart required to change voices.',
        },
      ],
    },
    {
      kind: 'code-cards',
      id: 'architecture',
      title: 'Architecture',
      cards: [
        {
          title: 'Signal flow',
          code: 'Claude / MCP Host\n  └─ voice-soundboard-mcp  →  .ogg/.wav\n       └─ tools/tts-bridge  (WebSocket :9200)\n            └─ UBridgeClient  (C++)\n                 ├─ UTtsAudioPlayer  →  UAudioComponent\n                 │    └─ UVisemeDriverFFT  →  5 bands\n                 └─ UExpressionMapper  →  morph weights',
        },
        {
          title: 'Speak command',
          code: '// Unreal → Bridge\n{\n  "type": "speak",\n  "text": "Hello, how can I help?",\n  "voice": "am_fenrir",\n  "emotion": "happy",\n  "intensity": 0.7\n}',
        },
        {
          title: 'Play response',
          code: '// Bridge → Unreal\n{\n  "type": "tts.play",\n  "path": "/tmp/tts_abc123.wav",\n  "durationMs": 1840,\n  "emotion": "happy",\n  "intensity": 0.7\n}',
        },
        {
          title: 'Scene setup',
          code: '// 1. Place PersonifyManager actor in level\n// 2. Drop a .vrm file into Content/Avatars/\n// 3. Press Play — bridge auto-starts\n//    avatar loads, WebSocket connects',
        },
      ],
    },
    {
      kind: 'data-table',
      id: 'status',
      title: 'Build Status',
      subtitle: 'Current scaffold state — v0.1.0 roadmap in progress.',
      columns: ['Feature', 'State'],
      rows: [
        ['VRM 0.x / 1.0 loading (VRM4U)', 'Scaffold'],
        ['TTS bridge (WebSocket)', 'Scaffold'],
        ['Viseme lipsync (5 bands)', 'Scaffold'],
        ['Emotion expressions', 'Scaffold'],
        ['Bridge auto-start on Play', 'Scaffold'],
        ['Morph target cache', 'Scaffold'],
      ],
    },
    {
      kind: 'api',
      id: 'stack',
      title: 'Stack',
      subtitle: 'Technologies used in the prototype.',
      apis: [
        {
          signature: 'Unreal Engine 5.7.3',
          description: 'C++ module (mcppersonify). Build with VRM4U and WebSockets modules enabled.',
        },
        {
          signature: 'VRM4U plugin',
          description: 'Runtime VRM avatar loading. Download VRM4U_5_7.zip from ruyo/VRM4U releases and extract into Plugins/.',
        },
        {
          signature: 'IWebSocket (built-in)',
          description: 'UBridgeClient uses the engine WebSockets module — no third-party networking library required.',
        },
        {
          signature: 'voice-soundboard-mcp',
          description: 'MCP server that synthesises speech and returns .wav/.ogg paths. Runs as a local process connected via the bridge.',
        },
        {
          signature: 'tools/tts-bridge (Node.js)',
          description: 'Thin WebSocket relay on port 9200. Translates MCP stdio to JSON messages that UBridgeClient can consume.',
        },
      ],
    },
  ],
};
