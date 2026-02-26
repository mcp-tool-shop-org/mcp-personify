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

**प्रोटोटाइप:** यूनियल इंजन 5 में एक वीआरएम अवतार, जिसमें वास्तविक समय में टेक्स्ट-टू-स्पीच (TTS) तकनीक के माध्यम से होंठों की गति और चेहरे के भावों का सिंक्रोनाइज़ेशन (समन्वयन) किया गया है।

यह वीआरएम अवतार प्रणाली है जो एक स्थानीय टेक्स्ट-टू-स्पीच (TTS) इंटरफेस से जुड़ी होती है, जिसका उपयोग भाषण संश्लेषण, होंठों की गति के साथ सिंक्रोनाइज़ेशन (लिपसिंक), और भावनाओं को व्यक्त करने के लिए किया जाता है। यह गॉडोट प्रोटोटाइप ([avatar-face-mvp](https://github.com/mcp-tool-shop-org/avatar-face-mvp)) से रूपांतरित किया गया है।

## स्थिति।

| विशेषता। | State |
| ज़रूर, मैं आपकी मदद कर सकता हूँ। कृपया वह अंग्रेजी पाठ प्रदान करें जिसका आप हिंदी में अनुवाद करवाना चाहते हैं। | "The quick brown fox jumps over the lazy dog."

"यह फुर्तीला भूरा लोमड़ी आलसी कुत्ते के ऊपर से कूदता है।" |
| वीआरएम 0.x / 1.0 का डाउनलोड (वीआरएम4यू)। | ढांचा। |
| टीटीएस ब्रिज (वेबसोकेट) | ढांचा। |
| विसेम लिपसिंक (5 बैंड)। | ढांचा। |
| भावनाओं की अभिव्यक्ति। | ढांचा। |
| ब्रिज ऑटो-स्टार्ट। | मचान। |
| मॉर्फ टारगेट कैश। | ढांचा। |

## स्टैक।

| घटक। | विकल्प। |
| कृपया वह अंग्रेजी पाठ प्रदान करें जिसका आप हिंदी में अनुवाद करवाना चाहते हैं। मैं उसका सटीक और उचित अनुवाद करने के लिए तैयार हूं। | ज़रूर, मैं आपकी मदद कर सकता हूँ। कृपया वह अंग्रेजी पाठ प्रदान करें जिसका आप हिंदी में अनुवाद करवाना चाहते हैं। |
| इंजन। | अनरियल इंजन 5.7.3. |
| VRM | [VRM4U](https://github.com/ruyo/VRM4U) प्लगइन। |
| वेबसॉकेट। | अंतर्निहित `IWebSocket` (वेबसोकेट्स मॉड्यूल)। |
| TTS | [वॉयस-साउंडबोर्ड-एमसीपी](https://github.com/mcp-tool-shop-org/voice-soundboard-mcp) - ब्रिज के माध्यम से। |
| Audio | `UAudioComponent` और आयाम-आधारित विसेम (ध्वनि इकाइयों)। |

## स्थापना।

### आवश्यक शर्तें।
- अनरियल इंजन 5.7 (दीर्घकालिक समर्थन संस्करण)
- नोड.जेएस 18 या उससे ऊपर का संस्करण
- `@mcptoolshop/voice-soundboard-mcp` को वैश्विक रूप से स्थापित किया गया होना चाहिए।

### 1. क्लोन करें और खोलें।
```bash
git clone https://github.com/mcp-tool-shop-org/mcp-personify.git
```
UE5 में `mcppersonify.uproject` फ़ाइल खोलें। यह आपको C++ मॉड्यूल को कंपाइल करने के लिए कहेगा।

### 2. वीआरएम4यू (VRM4U) प्लगइन स्थापित करें।
1. `VRM4U_5_7.zip` फ़ाइल को [VRM4U रिलीज़](https://github.com/ruyo/VRM4U/releases) से डाउनलोड करें।
2. `VRM4U/` फ़ोल्डर को `Plugins/` डायरेक्टरी में निकालें।
3. एडिटर को पुनरारंभ करें, और फिर "Edit > Plugins" में जाकर प्लगइन को सक्रिय करें।

### 3. ब्रिज (Bridge) के आवश्यक घटकों को स्थापित करें।
```bash
cd tools/tts-bridge
npm install
```

### 4. एक वीआरएम अवतार जोड़ें।
किसी भी `.vrm` फ़ाइल को `Content/Avatars/` फ़ोल्डर में रखें। लोडर स्वचालित रूप से पहली फ़ाइल को खोज लेगा।

### 5. दृश्य को तैयार करें।
1. अपने गेम के स्तर में एक `PersonifyManager` एक्टर जोड़ें (इसे "प्लेस एक्टर्स" पैनल में खोजें)।
2. "प्ले" बटन दबाएं - ब्रिज स्वचालित रूप से शुरू हो जाएगा, अवतार लोड होगा, और यह टेक्स्ट-टू-स्पीच (TTS) के लिए तैयार हो जाएगा।

## आर्किटेक्चर।

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

## स्रोत फ़ाइलें।

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

## ब्रिज प्रोटोकॉल।

**अनरियल -> ब्रिज:**
- `{ "type": "speak", "text": "...", "voice": "am_fenrir", "emotion": "happy", "intensity": 0.7 }`
- `{ "type": "status" }` — उपलब्ध आवाज़ों की जानकारी प्राप्त करें।
- `{ "type": "stop" }` — रोकें (बीच में रोकें)।

**ब्रिज -> अवास्तविक:**
- `{ "type": "tts.play", "path": "...", "durationMs": N, "emotion": "...", "intensity": N }`
- `{ "type": "tts.status", "voices": [...], "ready": true }`
- `{ "type": "aside.item", "meta": { "emotion": "...", "intensity": 0.5 } }`

## v0.1.0 के लिए विकास योजना।

- [ ] सी++ मॉड्यूल UE 5.7 में बिना किसी त्रुटि के संकलित होता है।
- [ ] VRM4U अवतार को लोड करता है और दृश्य में प्रदर्शित करता है।
- [ ] ब्रिज कनेक्ट होता है और टेक्स्ट-टू-स्पीच (TTS) ऑडियो प्राप्त करता है।
- [ ] विसेम (visemes) मुंह के आकार को नियंत्रित करते हैं।
- [ ] भावनात्मक अभिव्यक्तियाँ चालू/बंद होती हैं।
- [ ] ब्रिज "प्ले" पर स्वचालित रूप से शुरू होता है।
- [ ] एक-क्लिक पैकेज (.exe) पूरी तरह से काम करता है।

## लाइसेंस।

एमआईटी (MIT)
