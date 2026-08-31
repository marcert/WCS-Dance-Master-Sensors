#ifndef SOLO_H
#define SOLO_H

const char HTML_SOLO_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>WCS Solo-Training Dashboard</title>
    <style>
                :root {
            --bg-color: #0b0e14;
            --card-bg: rgba(20, 20, 30, 0.15);
            --accent-left: #00f0ff;
            --accent-right: #ff007f;
            --text-color: #f0f6fc;
            --ok-color: #2ea043;
            --warn-color: #d29922;
            --danger-color: #f85149;
        }

                body {
            background: transparent;
            color: var(--text-color);
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            margin: 0;
            padding: 12px;
            box-sizing: border-box;
            display: flex;
            flex-direction: column;
            gap: 12px;
            min-height: 100vh;
        }

        #camera-feed {
            position: fixed;
            top: 0;
            left: 0;
            width: 100vw;
            height: 100vh;
            object-fit: cover;
            z-index: -1;
        }

        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding-bottom: 8px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }

        h1 { margin: 0; font-size: 1.4rem; font-weight: 800; background: linear-gradient(90deg, var(--accent-left), var(--accent-right)); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }

        .audio-toggle {
            background: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            color: #fff;
            padding: 6px 12px;
            border-radius: 6px;
            cursor: pointer;
            font-weight: bold;
        }

        /* GRID LAYOUT */
        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
            gap: 12px;
        }

                .card {
            background: var(--card-bg);
            border: 1px solid rgba(255, 255, 255, 0.25);
            border-radius: 10px;
            padding: 6px 10px;
            backdrop-filter: none;
            -webkit-backdrop-filter: none;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
            text-shadow: 0 1px 3px rgba(0,0,0,0.9);
            min-width: 0;
        }

                .card-flash {
            animation: flashCard 0.3s ease-out;
        }

        @keyframes flashCard {
            0% { border-color: rgba(255, 255, 255, 0.8); box-shadow: 0 0 12px rgba(255, 255, 255, 0.4); }
            100% { border-color: rgba(255, 255, 255, 0.1); box-shadow: none; }
        }

        /* METRICS & GAUGES */
        .metric-value { font-size: 2rem; font-weight: bold; text-shadow: 0 1px 3px rgba(0,0,0,0.9); }
        .badge { display: inline-block; padding: 2px 8px; border-radius: 12px; font-size: 0.75rem; font-weight: bold; margin-left: 6px; text-shadow: 0 1px 3px rgba(0,0,0,0.9); white-space: nowrap; }
        .badge-green { background: var(--ok-color); color: #fff; }
        .badge-yellow { background: var(--warn-color); color: #000; }
        .badge-red { background: var(--danger-color); color: #fff; }

        /* CANVASES — absolute-fill inside #canvas-wrapper; pixel dims set by ResizeObserver */
        canvas {
            position: absolute;
            top: 0; left: 0;
            width: 100%;
            height: 100%;
            display: block;
        }

        .bar-container { height: 8px; background: rgba(255,255,255,0.1); border-radius: 4px; overflow: hidden; margin-top: 4px; }
        .bar-fill { height: 100%; width: 0%; transition: width 0.1s ease; }

        /* STANCE TIMELINE */
        .stance-timeline {
            display: flex;
            height: 30px;
            background: rgba(0,0,0,0.4);
            border-radius: 6px;
            overflow: hidden;
            position: relative;
            margin-top: 8px;
        }
        .stance-indicator { height: 100%; position: absolute; top: 0; }
        .stance-left { background: var(--accent-left); opacity: 0.8; }
        .stance-right { background: var(--accent-right); opacity: 0.8; }
        .stance-double { background: #e3b341; opacity: 0.9; }

        /* ====================================================
           CARD TITLE
           ==================================================== */
        .card-title {
            font-size: 0.8rem;
            color: #8b949e;
            margin-bottom: 6px;
            font-weight: 600;
        }

        /* ====================================================
           GRAPH CARD & CANVAS WRAPPER
           ==================================================== */
        #graphCard {
            display: flex;
            flex-direction: column;
            min-height: 0;
        }

        /* Sized to 160px in portrait; flex:1 in landscape via media query below */
        #canvas-wrapper {
            position: relative;
            height: 160px;
            background: rgba(0, 0, 0, 0.15);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 6px;
            overflow: hidden;
        }

        /* ====================================================
           MAIN LAYOUT WRAPPER (graph + metric cards)
           ==================================================== */
        #main-layout {
            display: flex;
            flex-direction: column;
            gap: 12px;
            flex: 1;
            min-height: 0;
        }

        /* ====================================================
           PORTRAIT — single-column, scrollable (default)
           ==================================================== */
        @media (orientation: portrait) {
            body { min-height: 100vh; }
            .dashboard-grid {
                grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
                gap: 12px;
            }
            .hud-spacer { display: none; }
        }

        /* ====================================================
           LANDSCAPE — left: graph (bottom-half) + right: 2×2 grid
           ==================================================== */
        @media (orientation: landscape) {
            body {
                min-height: unset;
                height: 100vh;
                overflow: hidden;
                padding: 6px 10px;
                gap: 6px;
            }
            header { padding-bottom: 4px; }
            h1 { font-size: 1.05rem; }
            .audio-toggle { padding: 4px 8px; font-size: 0.78rem; }
            .card-title { font-size: 0.72rem; margin-bottom: 3px; }

            /* Left (graph) + right (metric cards) side by side */
            #main-layout {
                flex-direction: row;
                gap: 8px;
                overflow: hidden;
            }

            /* Graph: bottom ~50% of left column; top half stays empty (camera visible) */
            #graphCard {
                flex: 0 0 45%;
                height: 52%;
                align-self: flex-end;
                overflow: hidden;
            }

            /* Canvas wrapper grows to fill graphCard */
            #canvas-wrapper {
                flex: 1;
                height: auto;
                min-height: 0;
            }

            /* Right column: 2×2 grid fills full height */
            .dashboard-grid {
                flex: 1;
                grid-template-columns: 1fr 1fr;
                grid-template-rows: 1fr 1fr;
                gap: 6px;
                align-content: stretch;
                overflow: hidden;
                min-height: 0;
            }

            .hud-spacer { display: block; }

            .card { padding: 5px 8px; height: 100%; box-sizing: border-box; }
            .metric-value { font-size: 1.4rem; }
            .stance-timeline { height: 22px; margin-top: 4px; }
            .bar-container { margin-top: 2px; }

            /* ADV: narrow graph to make room for grounding card */
            main.level-advanced #graphCard {
                flex: 0 0 26%;
            }
            main.level-advanced #groundingCard {
                display: flex !important;
                flex-direction: column;
                justify-content: space-around;
                flex: 0 0 17%;
                height: 52%;
                align-self: flex-end;
                overflow: hidden;
            }
        }

        /* --- LEVEL-BASED VISIBILITY --- */

        /* BEGINNER: only step direction + strikeBadge visible; hide Double Stance, ASI, jerk, lower badges */
        main.level-beginner #doubleStanceCard,
        main.level-beginner #asiCard,
        main.level-beginner .jerk-section,
        main.level-beginner .bar-container { display: none !important; }
        .step-lower-badges { display:grid; grid-template-columns:1fr 1fr; gap:3px; margin-top:3px; }
        .step-lower-badges .badge { display:block; text-align:center; margin-left:0; box-sizing:border-box; white-space:nowrap; overflow:hidden; font-size:min(2.9vw,11px); }

        /* BEGINNER: push sole visible card to bottom-right */
        main.level-beginner #stepCard { grid-column: 2; grid-row: 2; }

        /* INTERMEDIATE: step card full (with jerk + power push); Double Stance visible; hide ASI, load/roll badges */
        main.level-intermediate #asiCard,
        main.level-intermediate #loadBadge,
        main.level-intermediate #rollBadge { display: none !important; }

        /* INTERMEDIATE: push both visible cards to bottom row */
        main.level-intermediate #doubleStanceCard { grid-column: 1; grid-row: 2; }
        main.level-intermediate #stepCard         { grid-column: 2; grid-row: 2; }

        /* ADVANCED: everything visible — no rules needed */
        /* Grounding card: visible in ADV (any orientation) */
        main.level-advanced #groundingCard { display: flex !important; flex-direction: column; }

        /* --- PELVIS CARD --- */
        /* Hidden by default; JS sets display when sensor comes online */
        #pelvicCard { display: none; }

        /* Level gating for pelvis sub-sections */
        main.level-beginner     .pelvis-int,
        main.level-beginner     .pelvis-adv { display: none !important; }
        main.level-intermediate .pelvis-adv { display: none !important; }

        /* Lock pelvis card to top-left slot in landscape grid */
        @media (orientation: landscape) {
            #pelvicCard { grid-column: 1; grid-row: 1; }
        }

        /* Battery warning overlay */
        #battWarnDiv {
            display: none; position: fixed; bottom: 12px; left: 50%; transform: translateX(-50%);
            background: rgba(180,0,0,0.92); color: #fff; font-weight: bold;
            font-size: min(3.5vw, 14px); padding: 5px 14px; border-radius: 20px;
            z-index: 9999; pointer-events: none; white-space: nowrap;
            animation: battBlink 0.8s infinite;
        }
        @keyframes battBlink { 0%,100%{opacity:1} 50%{opacity:0.15} }
    </style>
</head>
<body>

        <video id="camera-feed" autoplay playsinline></video>
        <div id="battWarnDiv">⚡ AKKU SCHWACH</div>

                <header>
            <h1>🕺 WCS Solo-Training Dashboard</h1>
            <div style="display: flex; gap: 8px;">
                <button id="camBtn" class="audio-toggle" style="background: rgba(0, 122, 255, 0.6);" onclick="startCamera()">📷 CAM</button>
                <button id="flipBtn" class="audio-toggle" style="background: rgba(255, 149, 0, 0.6); display: none;" onclick="flipCamera()">🔄 FLIP</button>
                <button id="fullBtn" class="audio-toggle" style="background: rgba(80, 80, 80, 0.6);" onclick="toggleFullscreen()">⛶ FULL</button>
                <button id="tareBtn" class="audio-toggle" style="background: rgba(0, 122, 255, 0.4);" onclick="tareFootAngles()">📐 ZERO</button>
                <button id="audioBtn" class="audio-toggle" onclick="toggleAudio()">🔇 Biofeedback: OFF</button>
                <button id="levelBtn" class="audio-toggle" style="background: rgba(46, 160, 67, 0.6);" onclick="cycleLevel()">👤 BEG</button>
                <button id="dbgBtn"   class="audio-toggle" style="background: rgba(80,80,80,0.5); display:none;" onclick="toggleDebug()">🔍 DBG</button>
            </div>
        </header>

<main id="main-layout">

        <!-- LIVE PITCH GRAPH -->
    <div class="card" id="graphCard">
        <div class="card-title">Live Roll-off Dynamics – Pitch Angular Velocity (&omega;_pitch)</div>
        <div id="canvas-wrapper"><canvas id="chartPitch"></canvas></div>
    </div>

    <!-- GROUNDING CARD (ADV only, sits next to graph in landscape) -->
    <div class="card" id="groundingCard" style="display:none;">
        <div class="card-title" style="font-size:min(2.8vw,11px);letter-spacing:.05em;">🌍 GROUNDING</div>
        <span id="sdrBadge"        class="badge" style="background:#1e272e;color:#8b949e;display:block;margin-top:3px;text-align:center;">— SDR</span>
        <span id="phaseDelayBadge" class="badge" style="background:#1e272e;color:#8b949e;display:block;margin-top:3px;text-align:center;">— SETTLE</span>
        <div id="groundingScore" style="margin-top:5px;font-size:min(3vw,13px);font-weight:bold;color:#aaa;text-align:center;">—</div>
        <div style="margin-top:3px;height:5px;background:#1e272e;border-radius:3px;overflow:hidden;">
            <div id="groundingBar" style="height:100%;width:0%;background:#aaa;border-radius:3px;transition:width 0.3s,background 0.3s;"></div>
        </div>
    </div>

    <!-- METRICS GRID -->
    <div class="dashboard-grid">

                                <!-- TOP-LEFT: empty spacer keeps camera HUD visible when pelvis is offline -->
                <div class="hud-spacer" id="pelvicSpacer"></div>

                <!-- TOP-LEFT: PELVIS HIP MECHANICS (replaces spacer while sensor is online) -->
                <div id="pelvicCard" class="card">
                    <div class="card-title">Pelvis — Hip Mechanics</div>
                    <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:3px;">
                        <span style="font-size:0.78rem;color:#8b949e;">Hip Activation</span>
                        <span id="hipActBadge" class="badge" style="background:#1e272e;color:#8b949e;">— HIP</span>
                    </div>
                    <div class="pelvis-int" style="display:flex;justify-content:space-between;align-items:center;margin-bottom:3px;">
                        <span style="font-size:0.78rem;color:#8b949e;">Lateral Stability</span>
                        <span id="slotBadge" class="badge" style="background:#1e272e;color:#8b949e;">— LATERAL</span>
                    </div>
                    <div class="pelvis-int" style="display:flex;justify-content:space-between;align-items:center;margin-bottom:3px;">
                        <span style="font-size:0.78rem;color:#8b949e;">Hip-Foot Coupling</span>
                        <span id="hipFootBadge" class="badge" style="background:#1e272e;color:#8b949e;">— COUPLING</span>
                    </div>
                    <div class="pelvis-int" style="display:flex;justify-content:space-between;align-items:center;margin-bottom:3px;">
                        <span style="font-size:0.78rem;color:#8b949e;">Vertical Bounce</span>
                        <span id="bounceBadge" class="badge" style="background:#1e272e;color:#8b949e;">— BOUNCE</span>
                    </div>
                    <div class="pelvis-int" style="display:flex;justify-content:space-between;align-items:center;margin-bottom:3px;">
                        <span style="font-size:0.78rem;color:#8b949e;">Pelvic Tilt</span>
                        <span id="pelvicTiltBadge" class="badge" style="background:#1e272e;color:#8b949e;">— TILT</span>
                    </div>
                    <div class="pelvis-adv" style="display:flex;justify-content:space-between;align-items:center;">
                        <span style="font-size:0.78rem;color:#8b949e;">Anchor Settle</span>
                        <span id="anchorSettleBadge" class="badge" style="background:#1e272e;color:#8b949e;">— ANCHOR</span>
                    </div>
                    <div class="pelvis-adv" style="display:flex;justify-content:space-between;align-items:center;">
                        <span style="font-size:0.78rem;color:#8b949e;">Hip Settle</span>
                        <span id="hipSettleBadge" class="badge" style="background:#1e272e;color:#8b949e;">— HIP SETTLE</span>
                    </div>
                </div>

                                <!-- TOP-RIGHT: BILATERAL OVERLAP (DOUBLE STANCE) -->
                <div class="card" id="doubleStanceCard">
                    <div class="card-title">Double Stance Overlap (&Delta;t double_stance)</div>
                    <div style="display: flex; justify-content: space-between; align-items: baseline;">
                        <div>
                            <span class="metric-value" id="doubleStanceVal">0 ms</span>
                            <span id="stanceRatioVal" style="font-size: 1.2rem; color: #8b949e; margin-left: 6px;">(0%)</span>
                        </div>
                        <span id="stanceBadge" class="badge badge-green">OPTIMAL ROLL</span>
                    </div>
                    <div class="stance-timeline">
                        <div id="stanceBarLeft" class="stance-indicator stance-left" style="width: 0%;"></div>
                        <div id="stanceBarDouble" class="stance-indicator stance-double" style="width: 0%;"></div>
                        <div id="stanceBarRight" class="stance-indicator stance-right" style="width: 0%;"></div>
                    </div>
                    <div id="dsDebugCard" style="display:none; margin-top:6px; font-size:min(3.5vw,14px); font-family:monospace; letter-spacing:0.03em;">
                        <span style="color:#4fc3f7;">L <strong id="dbgAzL2">0.00</strong>g <span id="dbgGndL2" style="font-size:1.1em;">○</span></span>
                        <span style="margin-left:14px; color:#ef5350;">R <strong id="dbgAzR2">0.00</strong>g <span id="dbgGndR2" style="font-size:1.1em;">○</span></span>
                        <span style="margin-left:14px; color:#ffa726;">acc:<strong id="dbgDsAcc2">0</strong>ms</span>
                    </div>
                </div>

                                <!-- BOTTOM-LEFT: ASI (SYMMETRY) & SMOOTHNESS -->
                <div class="card" id="asiCard">
                    <div class="card-title">Roll-off Symmetry (ASI) & Smoothness</div>
                    <div style="display: flex; justify-content: space-between; align-items: flex-start;">
                        <div style="flex: 0 0 50%;">
                            <div style="font-size: 0.8rem; color:#8b949e;">Symmetry Index (ASI):</div>
                            <div class="metric-value" id="asiVal">0 %</div>
                            <span id="asiBadge" class="badge badge-green">SYMMETRIC</span>
                        </div>
                        <div style="flex: 0 0 50%; text-align: right;">
                            <div style="font-size: 0.8rem; color:#8b949e;">Roll-Smoothness (100 = smooth):</div>
                            <div class="metric-value" id="smoothVal" style="font-size: 1.4rem;">0</div>
                            <span id="smoothBadge" class="badge badge-green">SMOOTH</span>
                        </div>
                    </div>
                </div>

                                <!-- BOTTOM-RIGHT: HEEL STRIKE & IMPACT JERK -->
                <div class="card" id="stepCard">
                    <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                        <div class="card-title" style="margin: 0;">Last Step (Heel/Toe-Strike & Jerk)</div>
                    </div>
                    <div id="debugRow" style="display:none; margin-bottom:5px; border-bottom:1px solid #2a2a3a; padding-bottom:4px; font-family:monospace; font-size:11px; color:#666;">
                        <div>
                            <span>dθ&nbsp;<strong id="dbgDirVal" style="color:#aaa;">—</strong>&nbsp;<span id="dbgDirSrc" style="color:#666;">[θ]</span></span>
                            <span style="margin-left:10px;">L&nbsp;<strong id="dbgAYL" style="color:#4fc3f7;">0.00</strong>&nbsp;R&nbsp;<strong id="dbgAYR" style="color:#ef5350;">0.00</strong></span>
                            <span style="margin-left:10px;">P&nbsp;gPitch&nbsp;<strong id="dbgPPitch" style="color:#ce93d8;">0.0</strong>&nbsp;gYaw&nbsp;<strong id="dbgPYaw" style="color:#ce93d8;">0.0</strong></span>
                        </div>
                        <div style="margin-top:3px; color:#555;">
                            <span>L&nbsp;z:<strong id="dbgLOff" style="color:#888;">0.0</strong>°&nbsp;θ:<strong id="dbgLTheta" style="color:#4fc3f7;">0.0</strong>°&nbsp;a:<strong id="dbgAccL" style="color:#4fc3f7;">0.0</strong>°</span>
                            <span style="margin-left:12px;">R&nbsp;z:<strong id="dbgROff" style="color:#888;">0.0</strong>°&nbsp;θ:<strong id="dbgRTheta" style="color:#ef5350;">0.0</strong>°&nbsp;a:<strong id="dbgAccR" style="color:#ef5350;">0.0</strong>°</span>
                        </div>
                        <div id="dbgDsRow" style="margin-top:3px; color:#555;">
                            <span>L&nbsp;aZ:<strong id="dbgAzL" style="color:#4fc3f7;">0.00</strong>g&nbsp;<span id="dbgGndL">○</span></span>
                            <span style="margin-left:12px;">R&nbsp;aZ:<strong id="dbgAzR" style="color:#ef5350;">0.00</strong>g&nbsp;<span id="dbgGndR">○</span></span>
                            <span style="margin-left:12px;">DS_acc:<strong id="dbgDsAcc" style="color:#ffa726;">0</strong>ms</span>
                        </div>
                        <div id="dbgPelvisRow" style="margin-top:3px; color:#555; display:none;">
                            <span style="color:#ce93d8;">P&nbsp;</span>
                            <span>sag:<strong id="dbgPAY" style="color:#ce93d8;">0.00</strong>g</span>
                            <span style="margin-left:8px;">lat:<strong id="dbgPAX" style="color:#ce93d8;">0.00</strong>g</span>
                            <span style="margin-left:8px;">vert:<strong id="dbgPAZ" style="color:#ce93d8;">0.00</strong>g</span>
                        </div>
                    </div>
                    <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 4px;">
                        <span id="dirBadge" class="badge" style="background:#30363d; font-size:0.85rem; white-space:nowrap;">—</span>
                        <div class="jerk-section" style="text-align: right; font-size: 0.9rem; color: #8b949e;">
                            Impact Jerk: <strong id="jerkVal" style="color:#fff;">0</strong> g/s
                        </div>
                    </div>
                    <div style="display:flex; align-items:center; justify-content:flex-end;">
                        <span id="strikeBadge" class="badge badge-green" style="min-width:7.5rem; text-align:center; display:inline-block;">OPTIMAL</span>
                    </div>
                    <div class="step-lower-badges">
                        <span id="powerBadge" class="badge" style="background:#1e272e; color:#8b949e;">— PUSH-OFF</span>
                        <span id="loadBadge"  class="badge" style="background:#1e272e; color:#8b949e;">— LOADING</span>
                        <span id="rollBadge"  class="badge" style="background:#1e272e; color:#8b949e;">— ANKLE ROLL</span>
                        <span id="delayBadge" class="badge" style="background:#1e272e; color:#8b949e;">— DELAY</span>
                        <span id="rollSmoothBadge" class="badge" style="background:#1e272e; color:#8b949e;">— ROLL</span>
                        <span id="ballHeelBadge" class="badge" style="background:#1e272e; color:#8b949e;">— BALL→HEEL</span>
                        <span id="heelBallBadge" class="badge" style="background:#1e272e; color:#8b949e; grid-column:1/-1;">— HEEL→BALL</span>
                    </div>
                </div>

    </div>

</main>

<script>
    // --- FULLSCREEN LOGIC ---
    const fullBtn = document.getElementById('fullBtn');
    function toggleFullscreen() {
        if (!document.fullscreenElement && !document.webkitFullscreenElement) {
            if (document.documentElement.requestFullscreen) document.documentElement.requestFullscreen();
            else if (document.documentElement.webkitRequestFullscreen) document.documentElement.webkitRequestFullscreen();
            if (fullBtn) fullBtn.innerText = "EXIT";
        } else {
            if (document.exitFullscreen) document.exitFullscreen();
            else if (document.webkitExitFullscreen) document.webkitExitFullscreen();
            if (fullBtn) fullBtn.innerText = "⛶ FULL";
        }
    }

    // --- CAMERA LOGIC (WITH FLIP) ---
    const videoElement = document.getElementById('camera-feed');
    const camBtn = document.getElementById('camBtn');
    const flipBtn = document.getElementById('flipBtn');

    let currentFacingMode = "environment";
    let activeStream = null;

    async function startCamera() {
        if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
            try {
                if (activeStream) {
                    activeStream.getTracks().forEach(track => track.stop());
                }
                activeStream = await navigator.mediaDevices.getUserMedia({
                    video: { facingMode: currentFacingMode }
                });

                videoElement.srcObject = activeStream;
                document.body.classList.add('cam-active');
                camBtn.style.display = 'none';
                flipBtn.style.display = 'inline-block';
            } catch (error) { 
                alert("Camera access failed: " + error.message); 
            }
        }
    }

    async function flipCamera() {
        currentFacingMode = (currentFacingMode === "environment") ? "user" : "environment";
        await startCamera();
    }

    // --- WEB AUDIO API (BIOFEEDBACK) ---
    let audioCtx = null;
    let audioEnabled = false;

        function toggleAudio() {
        if (!audioCtx) {
            audioCtx = new (window.AudioContext || window.webkitAudioContext)();
        }
        audioEnabled = !audioEnabled;
        document.getElementById('audioBtn').innerText = audioEnabled ? "🔊 Biofeedback: ON" : "🔇 Biofeedback: OFF";
    }

    function playImpactClick(freq = 800) {
        if (!audioEnabled || !audioCtx) return;
        let osc = audioCtx.createOscillator();
        let gain = audioCtx.createGain();
        osc.type = 'sine';
        osc.frequency.setValueAtTime(freq, audioCtx.currentTime);
        gain.gain.setValueAtTime(0.3, audioCtx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.01, audioCtx.currentTime + 0.08);
        osc.connect(gain);
        gain.connect(audioCtx.destination);
        osc.start();
        osc.stop(audioCtx.currentTime + 0.08);
    }

    function playLostAlert() {
        if (!audioCtx) return;
        // Two descending tones: 880 Hz → 440 Hz, 150 ms each — clearly distinct from impact click.
        [880, 440].forEach((freq, i) => {
            let osc = audioCtx.createOscillator();
            let gain = audioCtx.createGain();
            let t = audioCtx.currentTime + i * 0.18;
            osc.type = 'square';
            osc.frequency.setValueAtTime(freq, t);
            gain.gain.setValueAtTime(0.25, t);
            gain.gain.exponentialRampToValueAtTime(0.01, t + 0.15);
            osc.connect(gain);
            gain.connect(audioCtx.destination);
            osc.start(t);
            osc.stop(t + 0.15);
        });
    }

    // --- STREAM & DSP PROCESSING ---
    const canvas = document.getElementById('chartPitch');
    const ctx = canvas.getContext('2d');

    // Sync canvas pixel buffer to its CSS display size whenever the layout changes
    // (orientation switch, fullscreen toggle, window resize).
    const canvasRO = new ResizeObserver(() => {
        const w = canvas.clientWidth;
        const h = canvas.clientHeight;
        if (w > 0 && h > 0 && (canvas.width !== w || canvas.height !== h)) {
            canvas.width  = w;
            canvas.height = h;
            drawPitchChart();
        }
    });
    canvasRO.observe(canvas);

    const maxHistory = 100;
    let pitchLeftHistory  = new Array(maxHistory).fill(0);
    let pitchRightHistory = new Array(maxHistory).fill(0);
    let gYawHistory       = new Array(maxHistory).fill(0);

        let prevAccelZLeft = 1.0;
        let prevAccelZRight = 1.0;
        let prevGyroPitchLeft = 0.0;
        let prevGyroPitchRight = 0.0;
        let pitchLeftAngleRaw  = -28.0; // Left sensor aY is physically inverted: rest angle = atan2(-aYL, aZL) ≈ -28°
        let pitchRightAngleRaw =   0.0; // Right sensor rests at ~0° with atan2(aY, aZ) formula

                let lastStepTimeLeft = 0;
                let lastStepTimeRight = 0;
                let lastActiveFoot = "";
                let leftWasOnGround = false, rightWasOnGround = false;
                let leftLandedAt = 0, rightLandedAt = 0;
                let lastPowerPushTime = 0;
                let prevPitchLeftAngle  = -28.0; // T-1 CF angle saved before each frame's CF update
                let prevPitchRightAngle =   0.0;

                // Instep pitch offsets
        let leftMountOffset  = -28.0; // Left sensor: aY inverted, rest angle ≈ -28° with atan2(-aYL, aZL)
        let rightMountOffset =   0.0; // Right sensor neutral = 0° with atan2(aYR, aZR)

                let doubleStanceMs = 0;
        let currentStepOverlap = 0;
        let dsEpisodeStart = 0, dsEpisodeActive = false;
        let lastCompletedDsMs = 0, dsEpisodeEndTime = 0;
        let dsEpisodeGated = false; // true after timeout fires — blocks restart until next step clears it
        let peakDsThisStep = 0;    // max DS episode ms since last step event
        let lastStepTimestamp = Date.now();
        let stepDurationMs = 500; // Standard 500ms ~= 120 BPM
        let prevLeftOk = true, prevRightOk = true; // for sensor-lost alert
        let phantomSuppressCount = 0;
        // -- delay ramp monitor (tempo-normalised weight transfer timing) --
        let delayMonActive = false, delayMonFoot = null, delayMonDir = null;
        let delayMonStartTime = 0, delayMonConsec = 0;

                function tareFootAngles() {
                    // Use the live accel-snapshot angle (not the CF angle) so the offset is
                    // immune to gyro drift that may have accumulated during active movement.
                    leftMountOffset  = lastAccelAngleL;
                    rightMountOffset = lastAccelAngleR;
                    // Sync CF integration to the same reference so the graph re-centres too.
                    pitchLeftAngleRaw  = leftMountOffset;
                    pitchRightAngleRaw = rightMountOffset;
                    // Also zero the pelvis sagittal tilt
                    pelvicPitchOffset   = Math.atan2(lastPelvicSagP, lastPelvicVertP) * (180 / Math.PI);
                    pelvicPitchSmoothed = 0;
        
            let btn = document.getElementById('tareBtn');
            btn.innerText = "📐 ZEROED! ✓";
            btn.style.background = "rgba(46, 160, 67, 0.8)";
            setTimeout(() => {
                btn.innerText = "📐 ZERO";
                btn.style.background = "rgba(0, 122, 255, 0.4)";
            }, 1500);
        }

                let smoothnessBuffer = [];
        let smoothnessAvg = 0;
        let stanceBuffer = new Array(100).fill(0);
        let asiSmoothed = 0;
        let loadingSamples = []; let loadingActive = false; let loadingFoot = "";
        let rollSamples    = []; let rollActive    = false; let rollFoot    = "";
        let brushPending = false; let brushPendingTime = 0; let brushPendingFoot = "";

        // Pre-contact pitch-angle ring buffers — 10 samples at 50 Hz = 200 ms history per foot.
        // Used to resolve direction in the ambiguous θ zone via pitch-angle trend (dθ).
        // Forward step: θ rises (dorsiflexion during swing) → positive trend.
        // Backward step: θ falls (plantarflexion during swing) → negative trend.
        // Both feet use the calibrated θ convention — no left/right sign inversion required.
        let thetaBufferL = [];
        let thetaBufferR = [];

        // CF filter warmup — suppress step detection for 2 s after page load
        // while the complementary filter converges to the physical mount angle.
        const cfWarmupUntil = Date.now() + 2000;

        // Debug mode state
        let lastDirVal  = null;   // most recent direction decision value shown in debug row
        let lastDirSrc  = "θ";
        let debugMode   = false;

        // Last confirmed step direction per foot — used to set directional push-off threshold.
        // A foot that last stepped BACKWARD is now trailing in a forward walk → needs higher push threshold.
        // A foot that last stepped FORWARD is trailing in backward walk / anchor → lower threshold applies.
        let lastDirectionL = "FORWARD";
        let lastDirectionR = "FORWARD";

        // Push-off energy integrals — accumulated each frame while aY > 0.15g, reset when that foot lands.
        // Captures total angular impulse during push-off (°), complementing instantaneous peak detection.
        let pushIntegralL = 0;
        let pushIntegralR = 0;

        // Last accel-snapshot angles — updated every poll cycle, used by tare instead of CF angle
        let lastAccelAngleL = 0;
        let lastAccelAngleR = 0;

        // --- PELVIS METRICS STATE ---
        let gYawAbsHistory = new Array(25).fill(0);   // rolling |gYawP| — 25 frames = 500ms, for hip activation
        let aXPHistory     = new Array(50).fill(0);   // rolling aXP — 50 frames = 1s, for slot adherence
        let aZPDynHistory  = new Array(50).fill(0);   // rolling (aZP−1) — 1s, for vertical bounce
        let gYawTimedBuf   = [];                      // {t, v} pairs — last 600ms, for hip-foot coupling
        let hipActSmoothed = 0;                       // IIR-smoothed hip activation amplitude
        let pelvicPitchOffset   = 0;                  // zeroed at ZERO press, sagittal tilt reference
        let pelvicPitchSmoothed = 0;                  // IIR-smoothed pelvis pitch deviation
        let lastPelvicSagP  = 0, lastPelvicVertP = 1.0; // last known accel values for tare

        // --- Grounding metrics state ---
        let prevAVertP_g = 1.0;
        let sdrActive = false, sdrStartTime = 0, sdrFootJerk = 0, sdrPelvisJerkPeak = 0;
        let pdActive = false, pdStartTime = 0, pdMinAz = 2.0, pdMinTime = 0;
        let rsmActive = false, rsmStartTime = 0, rsmActiveFoot = "L", rsmSum = 0, rsmCount = 0, rsmPrevGPitch = 0;

        let anchorSettleActive       = false;            // true while collecting post-anchor pelvis window
        let anchorSettleStartTime    = 0;
        let anchorSettleLastTrigger  = 0;              // updated on each backward step; evaluates 500ms after the LAST backward step
        let anchorSettleHoldUntil    = 0;              // score stays visible until this time; no new MEASURING triggers during hold
        let anchorSettleBwdCount     = 0;              // consecutive backward steps in current window; evaluation only if >= 3 (anchor triple)
        let anchorWindowMs          = 500;            // Ball→Heel window: tempo-adaptive, capped 280–900 ms (1.05× step)
        let anchorSettleWindowMs    = 700;            // min gap after last backward step before Anchor Settle evaluates; >500ms so beat-& at 60BPM can extend the deadline before timer fires
        let anchorSettleEvalCount   = 0;              // debug: increments each time the evaluation block is entered
        let anchorSettleLastScore   = -1;             // debug: last computed score (-1 = no score yet)
        let anchorSettleSamples   = { aSagP: [], gYawP: [], aLatP: [] };

        // Hitch & Go detection — brief foot lift on recently-placed foot
        // Ball-to-Heel anchor progression — foot θ during anchor window
        let anchorThetaActive = false, anchorThetaFoot = '', anchorThetaStart = 0;
        let anchorThetaSamples = [];
        let anchorThetaAtTrigger = 0;  // T-1 snapshot: actual foot angle at impact, pre-reset
        // Heel-to-Ball forward progression — foot θ during forward step window
        let heelBallActive = false, heelBallFoot = '', heelBallStart = 0, heelBallWindowMs = 500;
        let heelBallSamples = [];

        function fetchStream() {
            fetch('/data')
                .then(res => res.json())
                .then(data => {
                    let dt = 0.02; // 20ms
                    let now = Date.now();

                                        let gPitchL = data.lG   ?? 0;
                    let aZL     = data.lA   ?? 1.0;
                    let aYL     = data.lAy  ?? 0.0;
                    let gRollL  = data.lGr  ?? 0;
                    let aXL     = data.lAx  ?? 0.0;
                    let gPitchR = data.rG   ?? 0;
                    let aZR     = data.rA   ?? 1.0;
                    let aYR     = data.rAy  ?? 0.0;
                    let gRollR  = data.rGr  ?? 0;
                    let aXR     = data.rAx  ?? 0.0;
                    let leftOk   = data.lOk === true;
                    let rightOk  = data.rOk === true;
                    let pelvicOk = data.pOk === true;

                    // Battery warning: show blinking overlay if any active sensor ≤ 20%
                    { const BATT_WARN = 20;
                      let warns = [];
                      if ((data.mBatt??0) > 0 && data.mBatt <= BATT_WARN) warns.push('Master ' + data.mBatt + '%');
                      if (leftOk  && (data.lBatt??0) > 0 && data.lBatt <= BATT_WARN) warns.push('L ' + data.lBatt + '%');
                      if (rightOk && (data.rBatt??0) > 0 && data.rBatt <= BATT_WARN) warns.push('R ' + data.rBatt + '%');
                      if (pelvicOk&& (data.pBatt??0) > 0 && data.pBatt <= BATT_WARN) warns.push('Pelvis ' + data.pBatt + '%');
                      if (data.hOk && (data.hBatt??0) > 0 && data.hBatt <= BATT_WARN) warns.push('Hand ' + data.hBatt + '%');
                      let wDiv = document.getElementById('battWarnDiv');
                      if (wDiv) { if (warns.length) { wDiv.innerText = '⚡ AKKU SCHWACH: ' + warns.join(' · '); wDiv.style.display = 'block'; } else { wDiv.style.display = 'none'; } }
                    }

                    // Sensor-lost alert: play descending tone on offline transition
                    if (!leftOk  && prevLeftOk)  playLostAlert();
                    if (!rightOk && prevRightOk) playLostAlert();
                    prevLeftOk  = leftOk;
                    prevRightOk = rightOk;
                    let gPitchP  = data.pG   ?? 0;
                    let aVertP   = data.pA   ?? 1.0;   // accel_z = vertical  (+1.0g at rest)
                    let aSagP    = data.pAy  ?? 0.0;   // accel_y = sagittal  (anterior-posterior)
                    let gYawP    = data.pYaw ?? 0;
                    let aLatP    = data.pAx  ?? 0.0;   // accel_x = lateral

                                        // 1. Live Pitch Curve Buffer — low-pass filtered (α=0.25) to suppress vibration noise
                    const LP_ALPHA = 0.25;
                    let filtL = pitchLeftHistory[pitchLeftHistory.length  - 1] * (1 - LP_ALPHA) + gPitchL * LP_ALPHA;
                    let filtR = pitchRightHistory[pitchRightHistory.length - 1] * (1 - LP_ALPHA) + gPitchR * LP_ALPHA;
                    pitchLeftHistory.shift();  pitchLeftHistory.push(filtL);
                    pitchRightHistory.shift(); pitchRightHistory.push(filtR);
                    gYawHistory.shift(); gYawHistory.push(pelvicOk ? gYawP : 0);

                    // aY swing-phase ring buffers — update every poll before step detection.
                    // Left foot aY is stored raw (inversion applied at read time, not here).
                    if (thetaBufferL.length >= 10) thetaBufferL.shift();
                    thetaBufferL.push(pitchLeftAngleRaw  - leftMountOffset);
                    if (thetaBufferR.length >= 10) thetaBufferR.shift();
                    thetaBufferR.push(pitchRightAngleRaw - rightMountOffset);

                    // Complementary filter: gyro integration for short-term dynamics,
                    // accel angle for long-term drift correction (2% per frame at 50 Hz ≈ 1°/s max correction).
                    // α=0.94 (τ≈78ms). Impact-gated variants (α=0.985 / magnitude gate 0.3g/0.6g) both
                    // corrupted the T-8→T-1 dθ window — α=0.985 compressed dθ; magnitude gate fired during
                    // swing phase (1.5–2.5g). T-1 snapshot + angle reset at trigger already
                    // protect θ from impact corruption — no additional gate needed.
                    const CF_ALPHA = 0.94;
                    // Left sensor aY is physically inverted: negate aYL so heel-down gives positive angle
                    let accelAngleL = Math.atan2(-aYL, aZL) * (180 / Math.PI);
                    let accelAngleR = Math.atan2( aYR, aZR) * (180 / Math.PI);
                    lastAccelAngleL = accelAngleL; // keep fresh for tare
                    lastAccelAngleR = accelAngleR;
                    // Save T-1 CF angles before this frame's update — used for direction & theta at trigger
                    prevPitchLeftAngle  = pitchLeftAngleRaw;
                    prevPitchRightAngle = pitchRightAngleRaw;
                    if (leftOk) {
                        pitchLeftAngleRaw  = CF_ALPHA * (pitchLeftAngleRaw  + gPitchL * dt) + (1 - CF_ALPHA) * accelAngleL;
                    }
                    if (rightOk) {
                        pitchRightAngleRaw = CF_ALPHA * (pitchRightAngleRaw + gPitchR * dt) + (1 - CF_ALPHA) * accelAngleR;
                    }

                    // Push-off energy integrals — accumulate floor-shear impulse each frame.
                    // Gated by aY > 0.15g (floor contact); reset when foot lands (see step trigger block).
                    if (aYL > 0.15) pushIntegralL += Math.max(0, -gPitchL) * dt;
                    if (aYR > 0.15) pushIntegralR += Math.max(0, -gPitchR) * dt;

                    // --- PELVIS METRICS (per frame) ---
                    if (pelvicOk) {
                        document.getElementById('pelvicCard').style.display = 'block';
                        document.getElementById('pelvicSpacer').style.display = 'none';

                        // Hip Activation — rolling max of |gYawP| over 500ms, IIR-smoothed
                        gYawAbsHistory.shift(); gYawAbsHistory.push(Math.abs(gYawP));
                        let gYawPeak = Math.max(...gYawAbsHistory);
                        hipActSmoothed = hipActSmoothed * 0.9 + gYawPeak * 0.1;
                        let hipBadge = document.getElementById('hipActBadge');
                        if (hipBadge) {
                            let hipThrActive = Math.round(60 * 500 / Math.max(400, stepDurationMs));
                            let hipThrMod    = Math.round(25 * 500 / Math.max(400, stepDurationMs));
                            if      (hipActSmoothed >= hipThrActive) { hipBadge.className = 'badge badge-green';  hipBadge.style.cssText = ''; hipBadge.innerText = '🌀 ACTIVE'; }
                            else if (hipActSmoothed >= hipThrMod)    { hipBadge.className = 'badge badge-yellow'; hipBadge.style.cssText = ''; hipBadge.innerText = 'MODERATE'; }
                            else                                     { hipBadge.className = 'badge badge-red';    hipBadge.style.cssText = ''; hipBadge.innerText = 'STIFF HIPS'; }
                        }

                        // Lateral Stability — variance of aXP over 1s
                        aXPHistory.shift(); aXPHistory.push(aLatP);
                        let aXMean = aXPHistory.reduce((a,b)=>a+b,0) / aXPHistory.length;
                        let aXVar  = aXPHistory.reduce((a,b)=>a+(b-aXMean)**2,0) / aXPHistory.length;
                        let slotBadge = document.getElementById('slotBadge');
                        if (slotBadge) {
                            if      (aXVar < 0.004)  { slotBadge.className = 'badge badge-green';  slotBadge.style.cssText = ''; slotBadge.innerText = 'STABLE'; }
                            else if (aXVar < 0.015)  { slotBadge.className = 'badge badge-yellow'; slotBadge.style.cssText = ''; slotBadge.innerText = 'SLIGHT SWAY'; }
                            else                     { slotBadge.className = 'badge badge-red';    slotBadge.style.cssText = ''; slotBadge.innerText = 'LATERAL SWAY'; }
                        }

                        // Vertical Bounce — variance of dynamic aZP (gravity removed) over 1s
                        aZPDynHistory.shift(); aZPDynHistory.push(aVertP - 1.0);
                        let aZMean = aZPDynHistory.reduce((a,b)=>a+b,0) / aZPDynHistory.length;
                        let aZVar  = aZPDynHistory.reduce((a,b)=>a+(b-aZMean)**2,0) / aZPDynHistory.length;
                        let bounceBadge = document.getElementById('bounceBadge');
                        if (bounceBadge) {
                            if      (aZVar < 0.006)  { bounceBadge.className = 'badge badge-green';  bounceBadge.style.cssText = ''; bounceBadge.innerText = 'GROUNDED'; }
                            else if (aZVar < 0.030)  { bounceBadge.className = 'badge badge-yellow'; bounceBadge.style.cssText = ''; bounceBadge.innerText = 'SLIGHT BOUNCE'; }
                            else                     { bounceBadge.className = 'badge badge-red';    bounceBadge.style.cssText = ''; bounceBadge.innerText = 'BOUNCY'; }
                        }

                        // Pelvic Tilt — sagittal angle from accel, IIR-smoothed, zeroed at ZERO press
                        lastPelvicSagP  = aSagP;
                        lastPelvicVertP = aVertP;
                        let pelvicPitchRaw = Math.atan2(aSagP, aVertP) * (180 / Math.PI) - pelvicPitchOffset;
                        pelvicPitchSmoothed = pelvicPitchSmoothed * 0.92 + pelvicPitchRaw * 0.08;
                        let tiltBadge = document.getElementById('pelvicTiltBadge');
                        if (tiltBadge) {
                            let t = pelvicPitchSmoothed;
                            if      (Math.abs(t) <= 6) { tiltBadge.className = 'badge badge-green';  tiltBadge.style.cssText = ''; tiltBadge.innerText = 'ALIGNED'; }
                            else if (t >  6 && t <= 12){ tiltBadge.className = 'badge badge-yellow'; tiltBadge.style.cssText = ''; tiltBadge.innerText = 'SLIGHT ARCH'; }
                            else if (t > 12)           { tiltBadge.className = 'badge badge-red';    tiltBadge.style.cssText = ''; tiltBadge.innerText = 'LORDOSIS ⚠'; }
                            else                       { tiltBadge.className = 'badge badge-yellow'; tiltBadge.style.cssText = ''; tiltBadge.innerText = 'TUCKED'; }
                        }

                        // Hip-Foot Coupling timed ring — {t, v} pairs, 600ms window
                        gYawTimedBuf.push({ t: now, v: Math.abs(gYawP) });
                        while (gYawTimedBuf.length > 0 && now - gYawTimedBuf[0].t > 600) gYawTimedBuf.shift();

                        // Anchor Settle — collect samples only (timer evaluation is outside pelvicOk to survive brief sensor dropouts)
                        if (anchorSettleActive) {
                            anchorSettleSamples.aSagP.push(aSagP);
                            anchorSettleSamples.gYawP.push(Math.abs(gYawP));
                            anchorSettleSamples.aLatP.push(aLatP);
                        }
                    } else {
                        document.getElementById('pelvicCard').style.display = 'none';
                        document.getElementById('pelvicSpacer').style.display = '';
                    }

                    // Timer evaluation outside pelvicOk — survives brief sensor dropouts
                    if (anchorSettleActive && now - anchorSettleLastTrigger >= anchorSettleWindowMs) {
                        anchorSettleActive = false;
                        anchorSettleEvalCount++;
                        if (anchorSettleBwdCount >= 1) {
                            let n = anchorSettleSamples.aSagP.length;
                            if (n >= 3) {
                                let half = Math.floor(n / 2);
                                let earlyAY  = anchorSettleSamples.aSagP.slice(0, half);
                                let lateAY   = anchorSettleSamples.aSagP.slice(half);
                                let earlyYaw = anchorSettleSamples.gYawP.slice(0, half);
                                let lateYaw  = anchorSettleSamples.gYawP.slice(half);
                                let earlyRMS = Math.sqrt(earlyAY.reduce((a,b)=>a+b*b,0) / earlyAY.length);
                                let lateRMS  = Math.sqrt(lateAY.reduce((a,b)=>a+b*b,0)  / lateAY.length);
                                let settlingRatio = earlyRMS / (lateRMS + 0.01);
                                let decelScore = Math.min(1, Math.max(0, (settlingRatio - 1.0) / 1.5));
                                let earlyYawRMS = Math.sqrt(earlyYaw.reduce((a,b)=>a+b*b,0) / earlyYaw.length);
                                let lateYawRMS  = Math.sqrt(lateYaw.reduce((a,b)=>a+b*b,0)  / lateYaw.length);
                                let yawRatio     = earlyYawRMS / (lateYawRMS + 0.5);
                                let yawDampScore = Math.min(1, Math.max(0, (yawRatio - 1.0) / 1.5));
                                let lateYawM    = lateYaw.reduce((a,b)=>a+b,0) / lateYaw.length;
                                let lateYawVar  = lateYaw.reduce((a,b)=>a+(b-lateYawM)**2,0) / lateYaw.length;
                                let stabilScore = Math.max(0, 1 - lateYawVar / 400);
                                let anchorScore = Math.round((decelScore * 0.35 + yawDampScore * 0.35 + stabilScore * 0.30) * 100);
                                anchorSettleLastScore = anchorScore;
                                let ab = document.getElementById('anchorSettleBadge');
                                if (ab) {
                                    if      (anchorScore >= 50) { ab.className = 'badge badge-green';  ab.style.cssText = ''; ab.innerText = 'ANCHORED (' + anchorScore + ')'; }
                                    else if (anchorScore >= 30) { ab.className = 'badge badge-yellow'; ab.style.cssText = ''; ab.innerText = 'SETTLING (' + anchorScore + ')'; }
                                    else                        { ab.className = 'badge badge-red';    ab.style.cssText = ''; ab.innerText = 'UNSTABLE (' + anchorScore + ')'; }
                                }
                                let earlyLat = anchorSettleSamples.aLatP.slice(0, half);
                                let lateLat  = anchorSettleSamples.aLatP.slice(half);
                                let earlyLatPeak = Math.max(...earlyLat.map(v => Math.abs(v)));
                                let lateLatMean  = lateLat.reduce((a,b) => a+b, 0) / lateLat.length;
                                let lateLatVar   = lateLat.reduce((a,b) => a + (b - lateLatMean)**2, 0) / lateLat.length;
                                let hse = document.getElementById('hipSettleBadge');
                                if (hse) {
                                    hse.style.cssText = '';
                                    if      (earlyLatPeak > 0.10 && lateLatVar < 0.015)        { hse.className = 'badge badge-green';  hse.innerText = 'HIP SETTLE ✓'; }
                                    else if (earlyLatPeak > 0.30 && lateLatVar >= 0.015)        { hse.className = 'badge badge-yellow'; hse.innerText = 'OVERSWING ⚠'; }
                                    else if (earlyLatPeak > 0.05)                              { hse.className = 'badge badge-yellow'; hse.innerText = 'SLIGHT SETTLE'; }
                                    else                                                        { hse.className = 'badge badge-red';    hse.innerText = 'NO HIP SETTLE'; }
                                }
                                anchorSettleHoldUntil = now + 3000;
                            } else {
                                let ab = document.getElementById('anchorSettleBadge');
                                if (ab) { ab.className = 'badge'; ab.style.cssText = 'background:#1e272e;color:#8b949e;'; ab.innerText = '— ANCHOR'; }
                                anchorSettleHoldUntil = now + 3000; // hold to prevent immediate re-trigger
                            }
                        } else {
                            let ab = document.getElementById('anchorSettleBadge');
                            if (ab && ab.innerText === 'MEASURING...') { ab.className = 'badge'; ab.style.cssText = 'background:#1e272e;color:#8b949e;'; ab.innerText = '— ANCHOR'; }
                            anchorSettleHoldUntil = now + 3000; // hold even on walk-back cancel
                        }
                    }

                    // 2. STEP & HEEL/TOE-STRIKE DETECTION

                                        let triggerImpact = false;
                                        let activeFoot = "";
                                        let activeTheta = 0;
                                        let activeJerk = 0;
                                        let activeDirection = "FORWARD";

                                                                                // 1. Raw Transient Step Impact Sensing — offline sensors never trigger
                                                                                // gPitch trigger requires a minimum aZ change (preJerk > 8) to suppress zero-jerk
                                                                                // liftoff/rotation ghosts that have high gyro but no vertical impact signature.
                                                                                let preJerkL = Math.abs(aZL - prevAccelZLeft)  / 0.005;
                                                                                let preJerkR = Math.abs(aZR - prevAccelZRight) / 0.005;
                                                                                // Step trigger: tempo-adaptive threshold — slow practice (stepDurationMs>800ms, <75 BPM) uses 0.92g
                                                                                // to capture soft anchor ball-landings (aZ≈0.90–0.93g, preJerk≈3 g/s); dance tempo uses 0.95g to suppress brush artefacts.
                                                                                // preJerk>2.0 gate on aZ path suppresses stance-foot drift (preJerk~0.5–2); real impacts have preJerk>3.
                                                                                // Lockout (180–320 ms) + alternation guard remain the false-trigger protection.
                                                                                let aZThr = stepDurationMs > 800 ? 0.92 : 0.95;
                                                                                let leftSignal  = leftOk  && ((Math.abs(aZL) > aZThr && preJerkL > 2.0) || (Math.abs(gPitchL) > 80 && preJerkL > 8));
                                                                                let rightSignal = rightOk && ((Math.abs(aZR) > aZThr && preJerkR > 2.0) || (Math.abs(gPitchR) > 80 && preJerkR > 8));
                                                                                if (Date.now() < cfWarmupUntil) { leftSignal = false; rightSignal = false; }

                                                                                let detectedFoot = null;

                                                                                if (leftSignal && rightSignal) {
                                                                                    // Pick the foot with higher vertical ground-reaction force (landing foot has more |aZ|).
                                                                                    // |aZ| is more reliable than |gPitch| here: the swinging/pushing-off foot can have high gyro
                                                                                    // even when it is NOT the one making contact.
                                                                                    detectedFoot = (Math.abs(aZL) >= Math.abs(aZR)) ? "L" : "R";
                                                                                } else if (leftSignal) {
                                                                                    detectedFoot = "L";
                                                                                } else if (rightSignal) {
                                                                                    detectedFoot = "R";
                                                                                }

                                                                                // Opposing-foot plausibility: suppress brush/scuff phantom triggers.
                                                                                // Stance foot clearly loaded (>1.1g) while detected foot is unloaded (<0.90g) → no real weight transfer.
                                                                                if (detectedFoot === "L" && Math.abs(aZR) > 1.1 && Math.abs(aZL) < 0.90) { detectedFoot = null; phantomSuppressCount++; }
                                                                                if (detectedFoot === "R" && Math.abs(aZL) > 1.1 && Math.abs(aZR) < 0.90) { detectedFoot = null; phantomSuppressCount++; }

                                                                                // Global lockout: reject any trigger within 130 ms of the last confirmed step.
                                                                                // Per-foot lockout misses the case where the OPPOSITE foot false-triggers shortly after a
                                                                                // real step (e.g. R oscillates at ~1.0g and triggers 70 ms after real L: lastStepTimeRight
                                                                                // is stale, per-foot lockout does not block it). lastStepTimestamp is updated on every
                                                                                // confirmed step, so this catches cross-foot false triggers.
                                                                                if (detectedFoot !== null && (now - lastStepTimestamp) < 130) detectedFoot = null;

                                                                                                                                                                // 2. Per-Foot cadence-aware lockout + Alternation Guard
                                                                                // Lockout = 55% of the last measured step period, clamped 180–320 ms.
                                                                                // At rest (stepDurationMs=500): 275 ms. At 160 BPM (375 ms): 206 ms. At 200 BPM (300 ms): 180 ms.
                                                                                // Feet must alternate (L→R→L or R→L→R). Same foot firing twice without
                                                                                // the other foot in between = liftoff re-detection or vibration ghost.
                                                                                let lockoutMs = Math.max(180, Math.min(320, stepDurationMs * 0.55));
                                                                                if (detectedFoot === "L") {
                                                                                    if (now - lastStepTimeLeft < lockoutMs) {
                                                                                        detectedFoot = null; // hard lockout: too soon after last L event
                                                                                    } else {
                                                                                        lastStepTimeLeft = now;
                                                                                        if (lastActiveFoot === "L") {
                                                                                            detectedFoot = null; // alternation guard: L→L without R in between
                                                                                        }
                                                                                    }
                                                                                } else if (detectedFoot === "R") {
                                                                                    if (now - lastStepTimeRight < lockoutMs) {
                                                                                        detectedFoot = null; // hard lockout: too soon after last R event
                                                                                    } else {
                                                                                        lastStepTimeRight = now;
                                                                                        if (lastActiveFoot === "R") {
                                                                                            detectedFoot = null; // alternation guard: R→R without L in between
                                                                                        }
                                                                                    }
                                                                                }

                                                                                // 3. Process Verified Step Trigger & Polarity Logic
                                                                                if (detectedFoot === "L") {
                                                                                    lastActiveFoot = "L";
                                                                                    triggerImpact = true;
                                                                                    activeFoot = "L";
                                                                                    // Use T-1 CF angle (frame before trigger): captures pre-contact orientation
                                                                                    // before roll-through distorts the instantaneous accel angle at impact.
                                                                                    activeTheta = Math.round(prevPitchLeftAngle - leftMountOffset);
                                                                                    activeJerk = Math.abs((aZL - prevAccelZLeft)  / 0.005); // 0.005 s = native 200 Hz sensor step

                                                                                    let is_backward = (prevPitchLeftAngle < leftMountOffset);
                                                                                    activeDirection = is_backward ? "BACKWARD" : "FORWARD";
                                                                                    lastDirectionL = activeDirection;
                                                                                    pushIntegralL = 0; // L just landed — reset its push-off accumulator
                                                                                }
                                                                                else if (detectedFoot === "R") {
                                                                                    // RIGHT FOOT
                                                                                    lastActiveFoot = "R";
                                                                                    triggerImpact = true;
                                                                                    activeFoot = "R";
                                                                                    // Use T-1 CF angle — same principle as left foot.
                                                                                    activeTheta = Math.round(prevPitchRightAngle - rightMountOffset);
                                                                                    activeJerk = Math.abs((aZR - prevAccelZRight) / 0.005); // 0.005 s = native 200 Hz sensor step

                                                                                    let is_backward = (prevPitchRightAngle < rightMountOffset);
                                                                                    activeDirection = is_backward ? "BACKWARD" : "FORWARD";
                                                                                    lastDirectionR = activeDirection;
                                                                                    pushIntegralR = 0; // R just landed — reset its push-off accumulator
                                                                                }

                                        // --- Grounding: open measurement windows ---
                                        if (triggerImpact) {
                                            if (pelvicOk) {
                                                sdrActive = true; sdrStartTime = now; sdrFootJerk = activeJerk; sdrPelvisJerkPeak = 0;
                                                pdActive = true;  pdStartTime = now;  pdMinAz = aVertP; pdMinTime = now;
                                            }
                                            rsmActive = true; rsmStartTime = now; rsmActiveFoot = activeFoot;
                                            rsmSum = 0; rsmCount = 0; rsmPrevGPitch = (activeFoot === "L") ? gPitchL : gPitchR;
                                        }

                                        // 4. TERMINAL STANCE / POWER PUSH DETECTION (Windlass Push-off from Trailing Foot)
                                        // Directional thresholds: a foot that last stepped BACKWARD is trailing in a forward walk
                                        // → needs more drive (≥200°/s). A foot that last stepped FORWARD is trailing in a
                                        // backward walk / anchor redistribution → subtler push sufficient (≥160°/s).
                                        // Detection gate: aY > 0.15g confirms floor shear (filters unweighted foot swings).
                                        // Two complementary signals: instantaneous peak (catches short explosive pushes) and
                                        // energy integral accumulated since last landing (catches sustained lower-amplitude drives).
                                        let   sfPush            = 500 / Math.max(400, stepDurationMs);
                                        const PUSH_DETECT       = Math.round(120 * sfPush);
                                        const PUSH_OPT_FWD      = Math.round(200 * sfPush);
                                        const PUSH_OPT_BWD      = Math.round(160 * sfPush);
                                        const PUSH_INT_DETECT   = 12;   // integral (°) — tempo-independent
                                        const PUSH_INT_OPT_FWD  = 20;   // integral (°) — tempo-independent
                                        const PUSH_INT_OPT_BWD  = 16;   // integral (°) — tempo-independent
                                        let pushPeakL = aYL > 0.15 ? -gPitchL : 0;
                                        let pushPeakR = aYR > 0.15 ? -gPitchR : 0;
                                        let pushOptL    = (lastDirectionL === "BACKWARD") ? PUSH_OPT_FWD : PUSH_OPT_BWD;
                                        let pushOptR    = (lastDirectionR === "BACKWARD") ? PUSH_OPT_FWD : PUSH_OPT_BWD;
                                        let pushIntOptL = (lastDirectionL === "BACKWARD") ? PUSH_INT_OPT_FWD : PUSH_INT_OPT_BWD;
                                        let pushIntOptR = (lastDirectionR === "BACKWARD") ? PUSH_INT_OPT_FWD : PUSH_INT_OPT_BWD;
                                        let pushLevelPeakL = (pushPeakL >= pushOptL)        ? 2 : (pushPeakL >= PUSH_DETECT)     ? 1 : 0;
                                        let pushLevelPeakR = (pushPeakR >= pushOptR)        ? 2 : (pushPeakR >= PUSH_DETECT)     ? 1 : 0;
                                        let pushLevelIntL  = (pushIntegralL >= pushIntOptL) ? 2 : (pushIntegralL >= PUSH_INT_DETECT) ? 1 : 0;
                                        let pushLevelIntR  = (pushIntegralR >= pushIntOptR) ? 2 : (pushIntegralR >= PUSH_INT_DETECT) ? 1 : 0;
                                        let pushLevelL = Math.max(pushLevelPeakL, pushLevelIntL);
                                        let pushLevelR = Math.max(pushLevelPeakR, pushLevelIntR);
                                        let pushLevel  = Math.max(pushLevelL, pushLevelR);
                                        if (pushLevel > 0) lastPowerPushTime = now;
                                        let powerBadge = document.getElementById('powerBadge');
                                        if (powerBadge) {
                                            if (now - lastPowerPushTime < 400) {
                                                if (pushLevel === 2) {
                                                    powerBadge.className = "badge badge-green";
                                                    powerBadge.style.cssText = "";
                                                    powerBadge.innerText = "🚀 POWER PUSH";
                                                } else {
                                                    powerBadge.className = "badge badge-yellow";
                                                    powerBadge.style.cssText = "";
                                                    powerBadge.innerText = "↗ PUSH";
                                                }
                                            } else {
                                                powerBadge.className = "badge";
                                                powerBadge.style.cssText = "background:#1e272e; color:#8b949e;";
                                                powerBadge.innerText = "— PUSH-OFF";
                                            }
                                        }

                                        // Wenn ein Schritt gelandet ist -> UI & Richtungsbewertung sofort aktualisieren
                    if (triggerImpact) {
                                                // Determine step duration t_step since last step; update cadence estimate for lockout
                        let currentStepDuration = Math.max(200, Math.min(1500, now - lastStepTimestamp));
                        lastStepTimestamp = now;
                        // EMA smooth — prevents single anomalous interval from corrupting DS% denominator.
                        // α=0.55 (new sample weight) — fast convergence on tempo changes.
                        stepDurationMs = Math.round(stepDurationMs * 0.45 + currentStepDuration * 0.55);
                        // Reset both landedAt — timed-exit clock starts fresh for both feet from the step event.
                        leftLandedAt = now; rightLandedAt = now;

                        let dirBadge = document.getElementById('dirBadge');
                        let badge = document.getElementById('strikeBadge');
                        
                        // Guard against accel transients during rapid direction changes (e.g. 107° spike in v4 analysis).
                        // No valid WCS step angle exceeds ±45° — values outside that range are sensor artefacts.
                        activeTheta = Math.max(-45, Math.min(45, activeTheta));

                        // Reset debug source — overwritten below if dθ window fires.
                        lastDirVal = null;  lastDirSrc = "θ";

                        // Ambiguous zone (0° < θ < +10°): resolve direction from pitch-angle trend.
                        // Negative θ already confirms plantarflexion → BACKWARD, no trend check needed.
                        // Forward step: foot dorsiflexes during swing → θ rises → positive trend.
                        // Backward step: foot plantarflexes during swing → θ falls → negative trend.
                        // No left/right sign inversion required — both use same calibrated θ convention.
                        if (activeTheta > 0 && activeTheta < 10) {
                            let tBuf = (activeFoot === "L") ? thetaBufferL : thetaBufferR;
                            if (tBuf.length >= 9) {
                                let thetaTrend = tBuf[tBuf.length - 2] - tBuf[tBuf.length - 9]; // T-1 minus T-8
                                let trendThr = activeTheta > 5 ? 0.5 : 0.8;
                                if      (thetaTrend >  trendThr) activeDirection = "FORWARD";
                                else if (thetaTrend < -trendThr) activeDirection = "BACKWARD";
                                else                        activeDirection = "AMBIGUOUS";
                                lastDirVal = thetaTrend;  lastDirSrc = "dθ";
                            } else {
                                activeDirection = "AMBIGUOUS"; // buffer not yet warm
                                lastDirSrc = "dθ?";
                            }
                        }

                        // Update debug display (per-step values)
                        { let el = document.getElementById('dbgDirVal');
                          let sr = document.getElementById('dbgDirSrc');
                          if (el) el.innerText = lastDirVal !== null ? lastDirVal.toFixed(1) + "°" : "—";
                          if (sr) sr.innerText = "[" + lastDirSrc + "]"; }


                        document.getElementById('jerkVal').innerText = Math.round(activeJerk / 4); // ÷4 converts internal 200Hz-scaled value to actual g/s at poll rate

                                                // Direction badge: reliable only at θ-zone extremes
                                                if (activeTheta >= 6) {
                                                    dirBadge.innerText = "➡ FWD";
                                                    dirBadge.style.background = "#1f6beb";
                                                } else if (activeTheta < -6) {
                                                    dirBadge.innerText = "⬅ BWD";
                                                    dirBadge.style.background = "#a371f7";
                                                } else {
                                                    dirBadge.innerText = "—";
                                                    dirBadge.style.background = "#555";
                                                }

                                                // Strike badge: landing quality, direction-agnostic
                                                badge.style.cssText = "";
                                                if (activeTheta >= 6) {
                                                    if (activeJerk > 110) {
                                                        badge.className = "badge badge-red";    badge.innerText = "HEEL SLAM ⚠";
                                                        playImpactClick(1200);
                                                    } else {
                                                        badge.className = "badge badge-green";  badge.innerText = "HEEL STRIKE ✓";
                                                    }
                                                } else if (activeTheta < -6) {
                                                    if (activeJerk > 110) {
                                                        badge.className = "badge badge-red";    badge.innerText = "TOE JAM ⚠";
                                                        playImpactClick(1200);
                                                    } else {
                                                        badge.className = "badge badge-green";  badge.innerText = "TOE-FIRST ✓";
                                                    }
                                                } else {
                                                    // Ambiguous zone: quality only (all backward steps + flat forward steps)
                                                    if (activeJerk > 110) {
                                                        badge.className = "badge badge-red";    badge.innerText = "HARD IMPACT ⚠";
                                                        playImpactClick(1200);
                                                    } else if (activeJerk > 100) {
                                                        badge.className = "badge badge-yellow"; badge.innerText = "MODERATE";
                                                    } else {
                                                        badge.className = "badge badge-green";  badge.innerText = "SOFT ✓";
                                                    }
                                                    brushPending = true; brushPendingTime = now; brushPendingFoot = activeFoot;
                                                }

                        // Hip-Foot Coupling — compare when pelvis gYaw peaked vs foot impact time
                        // leadMs = ms since peak: large = hips moved well before foot landed (leads)
                        if (pelvicOk && gYawTimedBuf.length >= 3) {
                            let peak = gYawTimedBuf.reduce((a,b) => b.v > a.v ? b : a);
                            let leadMs = now - peak.t;
                            let hfBadge = document.getElementById('hipFootBadge');
                            if (hfBadge) {
                                if      (leadMs > 100) { hfBadge.className = 'badge badge-green';  hfBadge.style.cssText = ''; hfBadge.innerText = 'HIP LEADS'; }
                                else if (leadMs > 40)  { hfBadge.className = 'badge badge-yellow'; hfBadge.style.cssText = ''; hfBadge.innerText = 'IN SYNC'; }
                                else                   { hfBadge.className = 'badge badge-red';    hfBadge.style.cssText = ''; hfBadge.innerText = 'HIP LAGS'; }
                            }
                        }

                        // Anchor Settle — AMBIGUOUS steps extend the window only within the first 2000ms (anchor triple duration at 60BPM)
                        // After 2000ms the timer fires naturally; this prevents walk-steps classified as AMBIGUOUS from locking the window open indefinitely
                        if (anchorSettleActive && pelvicOk && activeDirection === "AMBIGUOUS") {
                            if (now - anchorSettleStartTime < 2000 && anchorSettleBwdCount < 2) {
                                anchorSettleLastTrigger = now; // extend deadline for beat& arriving within anchor triple (bwd<2 only)
                            }
                            // after bwd>=2 (beat6 done) or after 2000ms: ignore, let timer evaluate
                        }
                        // Only a clear FORWARD step force-evaluates (≥2 bwd) or cancels (<2 bwd)
                        if (anchorSettleActive && pelvicOk && activeDirection === "FORWARD") {
                            if (anchorSettleBwdCount >= 1) {
                                anchorSettleLastTrigger = 0; // forces evaluation on next setInterval tick
                            } else {
                                anchorSettleActive    = false;
                                anchorSettleBwdCount  = 0;
                                let ab = document.getElementById('anchorSettleBadge');
                                if (ab && ab.innerText === 'MEASURING...') { ab.className = 'badge'; ab.style.cssText = 'background:#1e272e;color:#8b949e;'; ab.innerText = '— ANCHOR'; }
                            }
                        }

                        // Anchor Settle — start on first backward step, extend deadline on each subsequent one
                        if (activeDirection === "BACKWARD" && now >= anchorSettleHoldUntil) {
                            if (!anchorSettleActive) {
                                anchorSettleActive    = true;
                                anchorSettleStartTime = now;
                                anchorSettleSamples   = { aSagP: [], gYawP: [], aLatP: [] };
                                anchorSettleBwdCount  = 0;
                            }
                            anchorSettleBwdCount++;
                            if (anchorSettleBwdCount <= 2) {
                                anchorSettleLastTrigger = now;  // extend deadline only for triple (beat5 + beat6); subsequent backward steps don't reopen window
                            }
                            anchorWindowMs = Math.min(900, Math.max(280, Math.round(stepDurationMs * 1.05)));
                            let ab = document.getElementById('anchorSettleBadge');
                            if (ab) { ab.className = 'badge'; ab.style.cssText = 'background:#1e272e;color:#8b949e;'; ab.innerText = 'MEASURING...'; }
                            let hse = document.getElementById('hipSettleBadge');
                            if (hse) { hse.className = 'badge'; hse.style.cssText = 'background:#1e272e;color:#8b949e;'; hse.innerText = 'MEASURING...'; }
                        }

                        // Ball-to-Heel anchor progression — always triggered on backward step (no pelvis required)
                        if (activeDirection === "BACKWARD") {
                            anchorThetaActive      = true;
                            anchorThetaFoot        = activeFoot;
                            anchorThetaStart       = now;
                            anchorThetaSamples     = [];
                            anchorThetaAtTrigger   = activeTheta;  // capture pre-reset T-1 angle
                            anchorWindowMs         = Math.min(900, Math.max(280, Math.round(stepDurationMs * 1.05)));
                            let bh = document.getElementById('ballHeelBadge');
                            if (bh) { bh.className = 'badge'; bh.style.cssText = 'background:#1e272e;color:#8b949e;'; bh.innerText = 'MEASURING...'; }
                            // Cancel any running forward progression — its result would be stale on a backward step
                            heelBallActive = false;
                            let hb2 = document.getElementById('heelBallBadge');
                            if (hb2) { hb2.className = 'badge'; hb2.style.cssText = 'background:#1e272e;color:#8b949e;'; hb2.innerText = '— HEEL→BALL'; }
                        }

                        // Heel-to-Ball progression — always triggered on forward step
                        if (activeDirection === "FORWARD") {
                            heelBallActive   = true;
                            heelBallFoot     = activeFoot;
                            heelBallStart    = now;
                            heelBallWindowMs = Math.min(500, Math.max(280, stepDurationMs));
                            heelBallSamples  = [];
                            let hb = document.getElementById('heelBallBadge');
                            if (hb) { hb.className = 'badge'; hb.style.cssText = 'background:#1e272e;color:#8b949e;'; hb.innerText = 'MEASURING...'; }
                            // Cancel any running backward progression — prevents BALL ONLY from a previous step overwriting this badge
                            anchorThetaActive = false;
                            let bh2 = document.getElementById('ballHeelBadge');
                            if (bh2) { bh2.className = 'badge'; bh2.style.cssText = 'background:#1e272e;color:#8b949e;'; bh2.innerText = '— BALL→HEEL'; }
                        }

                        // Ambiguous step — clear both progression badges (neither branch is meaningful)
                        if (activeDirection === "AMBIGUOUS") {
                            anchorThetaActive = false;
                            heelBallActive    = false;
                            let bh = document.getElementById('ballHeelBadge');
                            if (bh) { bh.className = 'badge'; bh.style.cssText = 'background:#1e272e;color:#8b949e;'; bh.innerText = '— BALL→HEEL'; }
                            let hb = document.getElementById('heelBallBadge');
                            if (hb) { hb.className = 'badge'; hb.style.cssText = 'background:#1e272e;color:#8b949e;'; hb.innerText = '— HEEL→BALL'; }
                        }

                        // Visuelles Aufblinken der Kachel bei jedem erkannten Schritt
                        let stepCard = document.getElementById('stepCard');
                        stepCard.classList.remove('card-flash');
                        void stepCard.offsetWidth; // Trigger Reflow
                        stepCard.classList.add('card-flash');


                        if (activeJerk > 120) playImpactClick(500); // threshold scaled to native 200 Hz dt (×4 vs 20 ms poll)

                        // Start post-impact monitoring windows — reset badges to pending
                        loadingSamples = []; loadingActive = true; loadingFoot = activeFoot;
                        rollSamples    = []; rollActive    = true; rollFoot    = activeFoot;
                        let lBadge = document.getElementById('loadBadge');
                        let rBadge = document.getElementById('rollBadge');
                        if (lBadge) { lBadge.className = "badge"; lBadge.style.cssText = "background:#1e272e;color:#8b949e;"; lBadge.innerText = "— LOADING"; }
                        if (rBadge) { rBadge.className = "badge"; rBadge.style.cssText = "background:#1e272e;color:#8b949e;"; rBadge.innerText = "— ANKLE ROLL"; }
                        // Start delay ramp monitor
                        delayMonActive = true; delayMonFoot = activeFoot; delayMonDir = activeDirection;
                        delayMonStartTime = now; delayMonConsec = 0;
                        let dBadge = document.getElementById('delayBadge');
                        if (dBadge) { dBadge.className = "badge"; dBadge.style.cssText = "background:#1e272e;color:#8b949e;"; dBadge.innerText = "— DELAY"; }
                    
                        // Force-close any running DS episode so it is not unboundedly accumulated
                        // across multiple steps (v12 fix for bimodal 0%/800%+ failure from v11).
                        if (dsEpisodeActive) {
                            lastCompletedDsMs = Math.min(now - dsEpisodeStart, stepDurationMs);
                            peakDsThisStep = Math.max(peakDsThisStep, lastCompletedDsMs);
                            dsEpisodeEndTime = now;
                            dsEpisodeActive = false;
                        }
                        dsEpisodeGated = false; // clear timeout gate — episode may start fresh after this step

                        // Max-hold: use peak DS reached in this step interval (prevents overwrite by later short episode).
                        let stanceRatio = Math.round((peakDsThisStep / stepDurationMs) * 100);
                        document.getElementById('doubleStanceVal').innerText = Math.round(peakDsThisStep) + " ms";
                        document.getElementById('stanceRatioVal').innerText = "(" + stanceRatio + "%)";
                        peakDsThisStep = 0; // reset for next interval

                                                let stanceBadge = document.getElementById('stanceBadge');
                        if (stanceRatio >= 15 && stanceRatio <= 60) {
                            stanceBadge.className = "badge badge-green"; stanceBadge.innerText = "OPTIMAL ROLL";
                        } else if (stanceRatio < 15) {
                            stanceBadge.className = "badge badge-yellow"; stanceBadge.innerText = "HECTIC";
                        } else {
                            stanceBadge.className = "badge badge-yellow"; stanceBadge.innerText = "SLUGGISH";
                        }

                        currentStepOverlap = 0; // Reset für neuen Schritt
                    }

                    // 3. Stance-Phasen & Überlappung — offline sensors never count as grounded
                    // Hysteresis: entry >0.65g, exit <exitAZ OR |gPitch|>80 OR |gRoll|>80.
                    // Gyro exit at 80°/s — normal push-off roll hits 60–75°/s,
                    // so 60°/s was still causing premature ○ at Count 4 / Count 1 (H1 confirmed in v22 analysis).
                    // v26 tested 90°/s but reverted: exits in practice happen via aZ path (heel unload), not gyro.
                    let minGnd = Math.max(150, Math.min(300, stepDurationMs * 0.4));
                    let exitAZL = (Math.abs(aZR) > 0.75) ? 0.48 : 0.45;
                    let exitAZR = (Math.abs(aZL) > 0.75) ? 0.48 : 0.45;
                    if (leftOk)  { if (Math.abs(aZL) > 0.65) { if (!leftWasOnGround)  leftLandedAt  = now; leftWasOnGround  = true; } else if ((Math.abs(aZL) < exitAZL || Math.abs(gPitchL) > 80 || Math.abs(gRollL) > 80) && (now - leftLandedAt)  > minGnd) leftWasOnGround  = false; } else { leftWasOnGround  = false; leftLandedAt  = 0; }
                    if (rightOk) { if (Math.abs(aZR) > 0.65) { if (!rightWasOnGround) rightLandedAt = now; rightWasOnGround = true; } else if ((Math.abs(aZR) < exitAZR || Math.abs(gPitchR) > 80 || Math.abs(gRollR) > 80) && (now - rightLandedAt) > minGnd) rightWasOnGround = false; } else { rightWasOnGround = false; rightLandedAt = 0; }
                    // Timed exit: if a foot has been ● for >75% of step interval since last step event, force ○.
                    // Handles aZR 0.50–0.85g "stuck ●" that bypasses both the aZ-exit (< 0.40g) and gyro-exit thresholds.
                    let maxGndMs = Math.max(350, Math.min(600, stepDurationMs * 0.75));
                    if (leftWasOnGround  && (now - leftLandedAt)  > maxGndMs) leftWasOnGround  = false;
                    if (rightWasOnGround && (now - rightLandedAt) > maxGndMs) rightWasOnGround = false;
                    let leftOnGround  = leftWasOnGround;
                    let rightOnGround = rightWasOnGround;

                    // A. Weight Transfer Gradient — 12 samples (~240 ms) after impact
                    // earlyMean vs lateMean: positive rise = progressive loading (smooth weight transfer)
                    if (loadingActive) {
                        let footAZ = Math.abs(loadingFoot === "L" ? aZL : aZR);
                        loadingSamples.push(footAZ);
                        if (loadingSamples.length >= 12) {
                            loadingActive = false;
                            let earlyMean = loadingSamples.slice(0, 4).reduce((a, b) => a + b, 0) / 4;
                            let lateMean  = loadingSamples.slice(8, 12).reduce((a, b) => a + b, 0) / 4;
                            let loadRise  = lateMean - earlyMean;
                            let lBadge = document.getElementById('loadBadge');
                            if (lBadge) {
                                if (loadRise > 0.12) {
                                    lBadge.className = "badge badge-green";  lBadge.style.cssText = ""; lBadge.innerText = "SMOOTH LOAD";
                                } else if (loadRise < -0.10) {
                                    lBadge.className = "badge badge-yellow"; lBadge.style.cssText = ""; lBadge.innerText = "EARLY UNLOAD";
                                } else {
                                    lBadge.className = "badge badge-yellow"; lBadge.style.cssText = ""; lBadge.innerText = "INSTANT LOAD";
                                }
                            }
                        }
                    }

                    // B. Ankle Shock Absorption + Rigid Lever — 10 samples (~200 ms) after impact
                    // First 100 ms (samples 0–4): pronation integral for shock absorption
                    // Samples 0–3 vs 6–9: sign reversal = supination lock (Rigid Lever for push-off)
                    if (rollActive) {
                        let footGRoll = rollFoot === "L" ? gRollL : gRollR;
                        rollSamples.push(footGRoll);
                        if (rollSamples.length >= 10) {
                            rollActive = false;
                            let rollIntegral = Math.abs(rollSamples.slice(0, 5).reduce((a, b) => a + b, 0) * 0.02);
                            let earlyRoll = rollSamples.slice(0, 4).reduce((a, b) => a + b, 0) / 4;
                            let lateRoll  = rollSamples.slice(6, 10).reduce((a, b) => a + b, 0) / 4;
                            let rigidLever = Math.abs(earlyRoll) > 8 && (earlyRoll * lateRoll < 0);
                            let rBadge = document.getElementById('rollBadge');
                            if (rBadge) {
                                if (rigidLever) {
                                    rBadge.className = "badge badge-green";  rBadge.style.cssText = ""; rBadge.innerText = "RIGID LEVER";
                                } else if (rollIntegral > 4) {
                                    rBadge.className = "badge badge-green";  rBadge.style.cssText = ""; rBadge.innerText = "ANKLE FLEX";
                                } else if (rollIntegral > 1) {
                                    rBadge.className = "badge badge-yellow"; rBadge.style.cssText = ""; rBadge.innerText = "MODERATE ROLL";
                                } else {
                                    rBadge.className = "badge badge-yellow"; rBadge.style.cssText = ""; rBadge.innerText = "STIFF ANKLE";
                                }
                            }
                        }
                    }

                    // C. Delay Ramp — tempo-normalised weight transfer timing
                    if (delayMonActive) {
                        let monAz = delayMonFoot === "L" ? aZL : aZR;
                        let monGy = delayMonFoot === "L" ? gPitchL : gPitchR;
                        if (Math.abs(monAz - 1.0) < 0.15 && Math.abs(monGy) < 40) {
                            delayMonConsec++;
                        } else {
                            delayMonConsec = 0;
                        }
                        let elapsed = now - delayMonStartTime;
                        if (delayMonConsec >= 2 || elapsed >= 500) {
                            delayMonActive = false;
                            let rampMs   = delayMonConsec >= 2 ? elapsed : 500;
                            let ratio    = rampMs / stepDurationMs;
                            let isFwd    = (delayMonDir === "FORWARD");
                            let quickThr = isFwd ? 0.12 : 0.18;
                            let lateThr  = isFwd ? 0.38 : 0.50;
                            let dBadge = document.getElementById('delayBadge');
                            if (dBadge) {
                                if (ratio < quickThr) {
                                    dBadge.className = "badge badge-yellow"; dBadge.style.cssText = ""; dBadge.innerText = "QUICK";
                                } else if (currentLevel === 'intermediate' || ratio <= lateThr) {
                                    dBadge.className = "badge badge-green";  dBadge.style.cssText = ""; dBadge.innerText = "DELAYED ✓";
                                } else {
                                    dBadge.className = "badge badge-yellow"; dBadge.style.cssText = ""; dBadge.innerText = "LATE";
                                }
                            }
                        }
                    }

                    // D. Brush+Heel reclassification — 200 ms window after flat/ambiguous forward contact
                    // If a second aZ peak (>1.05g) with positive accel angle (>8°) is detected on the
                    // same foot, the initial flat contact was the brush phase → reclassify to BRUSH+HEEL.
                    if (brushPending) {
                        if (now - brushPendingTime < 200) {
                            let footAZ    = Math.abs(brushPendingFoot === "L" ? aZL : aZR);
                            let footAngle = brushPendingFoot === "L"
                                ? (lastAccelAngleL - leftMountOffset)
                                : (lastAccelAngleR - rightMountOffset);
                            if (footAZ > 1.05 && footAngle > 8) {
                                brushPending = false;
                                let badge  = document.getElementById('strikeBadge');
                                let dirBdg = document.getElementById('dirBadge');
                                if (badge)  { badge.className = "badge badge-green"; badge.style.cssText = ""; badge.innerText = "BRUSH+HEEL"; }
                                if (dirBdg) { dirBdg.innerText = "➡ FWD"; dirBdg.style.background = "#1f6beb"; }
                            }
                        } else {
                            brushPending = false; // window expired — keep current quality badge
                        }
                    }

                    // F. Ball-to-Heel anchor progression — sample foot θ during anchor window
                    if (anchorThetaActive) {
                        let thetaAnch = anchorThetaFoot === 'L'
                            ? (pitchLeftAngleRaw  - leftMountOffset)
                            : (pitchRightAngleRaw - rightMountOffset);
                        anchorThetaSamples.push(thetaAnch);
                        if (now - anchorThetaStart >= anchorWindowMs) {
                            anchorThetaActive = false;
                            let n = anchorThetaSamples.length;
                            if (n >= 6) {
                                let half      = Math.floor(n / 2);
                                let earlyMean = anchorThetaAtTrigger;  // T-1 snapshot: pre-reset actual plantarflexion
                                let qStart    = Math.max(0, n - Math.max(3, Math.floor(n / 4)));
                                let lateMean  = anchorThetaSamples.slice(qStart).reduce((a, b) => a + b, 0) / (n - qStart);
                                let bh = document.getElementById('ballHeelBadge');
                                if (bh) {
                                    if      (earlyMean >= 0)                 { bh.className = 'badge badge-yellow'; bh.style.cssText = ''; bh.innerText = 'HEEL-FIRST'; }
                                    else if (earlyMean < -2 && lateMean > 3) { bh.className = 'badge badge-green';  bh.style.cssText = ''; bh.innerText = 'BALL→HEEL ✓'; }
                                    else if (earlyMean <  0 && lateMean > 0) { bh.className = 'badge badge-yellow'; bh.style.cssText = ''; bh.innerText = 'PARTIAL ROLL'; }
                                    else                                     { bh.className = 'badge badge-red';    bh.style.cssText = ''; bh.innerText = 'BALL ONLY ⚠'; }
                                }
                            }
                        }
                    }

                    // G. Heel-to-Ball forward progression — sample foot θ during forward step window
                    if (heelBallActive) {
                        let thetaFwd = heelBallFoot === 'L'
                            ? (pitchLeftAngleRaw  - leftMountOffset)
                            : (pitchRightAngleRaw - rightMountOffset);
                        heelBallSamples.push(thetaFwd);
                        if (now - heelBallStart >= heelBallWindowMs) {
                            heelBallActive = false;
                            let n = heelBallSamples.length;
                            if (n >= 6) {
                                let half      = Math.floor(n / 2);
                                let earlyMean = heelBallSamples.slice(0, half).reduce((a, b) => a + b, 0) / half;
                                let lateMean  = heelBallSamples.slice(half).reduce((a, b) => a + b, 0) / (n - half);
                                let drop = earlyMean - lateMean;   // positive = θ dropped (heel → ball)
                                let rise = lateMean - earlyMean;   // positive = θ rose   (ball → heel)
                                let hb = document.getElementById('heelBallBadge');
                                if (hb) {
                                    // Heel-lead forward step
                                    if      (earlyMean >  2 && drop >  3) { hb.className = 'badge badge-green';  hb.style.cssText = ''; hb.innerText = 'HEEL→BALL ✓'; }
                                    else if (earlyMean >  0 && drop >  1) { hb.className = 'badge badge-yellow'; hb.style.cssText = ''; hb.innerText = 'PARTIAL ROLL'; }
                                    else if (earlyMean >  2)              { hb.className = 'badge badge-red';    hb.style.cssText = ''; hb.innerText = 'HEEL STUCK ⚠'; }
                                    // Ball-lead forward step
                                    else if (earlyMean < -2 && rise >  3) { hb.className = 'badge badge-green';  hb.style.cssText = ''; hb.innerText = 'TOE→HEEL ✓'; }
                                    else if (earlyMean <  0 && rise >  1) { hb.className = 'badge badge-yellow'; hb.style.cssText = ''; hb.innerText = 'PARTIAL ROLL'; }
                                    else                                  { hb.className = 'badge badge-yellow'; hb.style.cssText = ''; hb.innerText = 'FLAT-FOOT'; }
                                }
                            }
                        }
                    }

                    // DS episode tracking — captures full ●● duration independent of step detection timing.
                    // This eliminates the ~100ms undercount from step detection firing late relative to first ground contact.
                    let bothGround = leftOnGround && rightOnGround;
                    if (bothGround && !dsEpisodeActive && !dsEpisodeGated) { dsEpisodeStart = now; dsEpisodeActive = true; }
                    if (!bothGround && dsEpisodeActive) { lastCompletedDsMs = now - dsEpisodeStart; peakDsThisStep = Math.max(peakDsThisStep, lastCompletedDsMs); dsEpisodeEndTime = now; dsEpisodeActive = false; dsEpisodeGated = false; }

                    // Hard timeout: if neither aZ nor gPitch/gRoll fires a natural exit (smooth glide),
                    // cap the episode at 38% of the previous step duration — the upper OPTIMAL boundary.
                    // Gate prevents episode from immediately restarting (which caused post-timeout force-close to
                    // capture only the tiny ~46ms since restart, showing 9% instead of the real value).
                    let maxDsMs = Math.max(100, stepDurationMs * 0.38);
                    if (dsEpisodeActive && (now - dsEpisodeStart) > maxDsMs) {
                        lastCompletedDsMs = maxDsMs;
                        peakDsThisStep = Math.max(peakDsThisStep, maxDsMs);
                        dsEpisodeEndTime = now;
                        dsEpisodeActive = false;
                        dsEpisodeGated = true; // block restart until next step event
                    }

                    if (leftOnGround && rightOnGround) {
                        currentStepOverlap += dt * 1000;
                    }

                    if (debugMode) {
                        let azLEl = document.getElementById('dbgAzL');
                        let azREl = document.getElementById('dbgAzR');
                        let gLEl  = document.getElementById('dbgGndL');
                        let gREl  = document.getElementById('dbgGndR');
                        let dsEl  = document.getElementById('dbgDsAcc');
                        if (azLEl) azLEl.innerText = Math.abs(aZL).toFixed(2);
                        if (azREl) azREl.innerText = Math.abs(aZR).toFixed(2);
                        if (gLEl)  { gLEl.innerText = leftOnGround  ? '●' : '○'; gLEl.style.color = leftOnGround  ? '#4fc3f7' : '#444'; }
                        if (gREl)  { gREl.innerText = rightOnGround ? '●' : '○'; gREl.style.color = rightOnGround ? '#ef5350' : '#444'; }
                        if (dsEl)  dsEl.innerText = dsEpisodeActive ? Math.round(now - dsEpisodeStart) : Math.round(lastCompletedDsMs);
                        let azL2 = document.getElementById('dbgAzL2');
                        let azR2 = document.getElementById('dbgAzR2');
                        let gL2  = document.getElementById('dbgGndL2');
                        let gR2  = document.getElementById('dbgGndR2');
                        let ds2  = document.getElementById('dbgDsAcc2');
                        if (azL2) azL2.innerText = Math.abs(aZL).toFixed(2);
                        if (azR2) azR2.innerText = Math.abs(aZR).toFixed(2);
                        if (gL2)  { gL2.innerText = leftOnGround  ? '●' : '○'; gL2.style.color = leftOnGround  ? '#4fc3f7' : '#444'; }
                        if (gR2)  { gR2.innerText = rightOnGround ? '●' : '○'; gR2.style.color = rightOnGround ? '#ef5350' : '#444'; }
                        if (ds2)  ds2.innerText = dsEpisodeActive ? Math.round(now - dsEpisodeStart) : Math.round(lastCompletedDsMs);
                    }

                    // Rolling 2-second proportional stance timeline (left / double / right)
                    let stanceState = 0;
                    if      (leftOnGround && rightOnGround) stanceState = 3;
                    else if (leftOnGround)                  stanceState = 1;
                    else if (rightOnGround)                 stanceState = 2;
                    stanceBuffer.shift(); stanceBuffer.push(stanceState);

                    let countLeft   = stanceBuffer.filter(s => s === 1).length;
                    let countDouble = stanceBuffer.filter(s => s === 3).length;
                    let countRight  = stanceBuffer.filter(s => s === 2).length;
                    let bufTotal    = stanceBuffer.length;

                    let leftPct   = (countLeft   / bufTotal * 100).toFixed(1);
                    let doublePct = (countDouble / bufTotal * 100).toFixed(1);
                    let rightPct  = (countRight  / bufTotal * 100).toFixed(1);

                    let leftBarEl   = document.getElementById('stanceBarLeft');
                    let doubleBarEl = document.getElementById('stanceBarDouble');
                    let rightBarEl  = document.getElementById('stanceBarRight');
                    leftBarEl.style.left    = "0%";
                    leftBarEl.style.width   = leftPct + "%";
                    doubleBarEl.style.left  = leftPct + "%";
                    doubleBarEl.style.width = doublePct + "%";
                    rightBarEl.style.left   = (parseFloat(leftPct) + parseFloat(doublePct)).toFixed(1) + "%";
                    rightBarEl.style.width  = rightPct + "%";

                    // 4. ASI (Abroll-Symmetrie) & GEGLÄTTETE Roll-Smoothness
                    // Only recalculate while either foot is actively moving — prevents noise accumulation during static standing
                    if (Math.abs(gPitchL) > 15 || Math.abs(gPitchR) > 15) {
                        let intL = pitchLeftHistory.reduce((a, b) => a + Math.abs(b), 0);
                        let intR = pitchRightHistory.reduce((a, b) => a + Math.abs(b), 0);
                        let intSum = intL + intR;
                        let asiRaw = (intSum > 0) ? (2 * Math.abs(intL - intR) / intSum) * 100 : 0;
                        // IIR smoothing (α=0.15): suppresses single-swing asymmetry spikes while tracking real bilateral imbalance
                        asiSmoothed = asiSmoothed * 0.85 + Math.min(100, asiRaw) * 0.15;
                        let asiRounded = Math.round(asiSmoothed);
                        document.getElementById('asiVal').innerText = asiRounded + " %";
                        let asiBadge = document.getElementById('asiBadge');
                        if (asiBadge) {
                            if (asiRounded <= 15)      { asiBadge.className = "badge badge-green";  asiBadge.innerText = "SYMMETRIC"; }
                            else if (asiRounded <= 35) { asiBadge.className = "badge badge-yellow"; asiBadge.innerText = "MINOR ASYM"; }
                            else                       { asiBadge.className = "badge badge-red";    asiBadge.innerText = "ASYMMETRIC"; }
                        }
                    }

                                        // Smoothness calculation with moving average buffer (over 25 frames / 0.5s)
                    let dOmegaL = (gPitchL - prevGyroPitchLeft)  / dt;
                    let dOmegaR = (gPitchR - prevGyroPitchRight) / dt;
                    let rawJerkiness = Math.min(100, Math.round((Math.abs(dOmegaL) + Math.abs(dOmegaR)) * 0.018));
                    let rawSmoothness = 100 - rawJerkiness; // invert: higher = smoother

                    smoothnessBuffer.push(rawSmoothness);
                    if (smoothnessBuffer.length > 25) smoothnessBuffer.shift();

                    smoothnessAvg = Math.round(smoothnessBuffer.reduce((a, b) => a + b, 0) / smoothnessBuffer.length);
                    document.getElementById('smoothVal').innerText = smoothnessAvg;
                    let smoothBadge = document.getElementById('smoothBadge');
                    if (smoothBadge) {
                        if (smoothnessAvg >= 65)      { smoothBadge.className = "badge badge-green";  smoothBadge.innerText = "SMOOTH"; }
                        else if (smoothnessAvg >= 40) { smoothBadge.className = "badge badge-yellow"; smoothBadge.innerText = "MODERATE"; }
                        else                          { smoothBadge.className = "badge badge-red";    smoothBadge.innerText = "ROUGH"; }
                    }

                    // === GROUNDING METRICS (per-frame evaluation) ===
                    if (pelvicOk) {
                        let pelvisJerkNow = Math.abs(aVertP - prevAVertP_g) / 0.005;
                        if (sdrActive) {
                            sdrPelvisJerkPeak = Math.max(sdrPelvisJerkPeak, pelvisJerkNow);
                            if (now - sdrStartTime >= 100) {
                                sdrActive = false;
                                let sdr = sdrFootJerk > 4 ? Math.max(0, Math.min(1, 1 - sdrPelvisJerkPeak / sdrFootJerk)) : -1;
                                let el = document.getElementById('sdrBadge');
                                if (el && sdr >= 0) {
                                    el.style.cssText = 'display:block;margin-top:3px;text-align:center;';
                                    if      (sdr > 0.65) { el.className='badge badge-green';  el.innerText='ABSORBING ✓'; }
                                    else if (sdr > 0.35) { el.className='badge badge-yellow'; el.innerText='PARTIAL SDR'; }
                                    else                 { el.className='badge badge-red';    el.innerText='STIFF'; }
                                }
                            }
                        }
                        if (pdActive) {
                            if (aVertP < pdMinAz) { pdMinAz = aVertP; pdMinTime = now; }
                            if (now - pdStartTime >= 200) {
                                pdActive = false;
                                if (pdMinTime > pdStartTime) {
                                    let dt = pdMinTime - pdStartTime;
                                    let el = document.getElementById('phaseDelayBadge');
                                    if (el) {
                                        el.style.cssText = 'display:block;margin-top:3px;text-align:center;';
                                        if      (dt >= 60 && dt <= 180) { el.className='badge badge-green';  el.innerText='SETTLING ✓ '+dt+'ms'; }
                                        else if (dt < 60)               { el.className='badge badge-yellow'; el.innerText='QUICK '+dt+'ms'; }
                                        else                            { el.className='badge badge-yellow'; el.innerText='SLOW '+dt+'ms'; }
                                    }
                                }
                            }
                        }
                        prevAVertP_g = aVertP;
                    }
                    if (rsmActive) {
                        let curGP = rsmActiveFoot === "L" ? gPitchL : gPitchR;
                        let dOmega = curGP - rsmPrevGPitch;
                        rsmSum += dOmega * dOmega;
                        rsmCount++;
                        rsmPrevGPitch = curGP;
                        if (now - rsmStartTime >= 350) {
                            rsmActive = false;
                            let rollVar = rsmCount > 0 ? rsmSum / rsmCount : 0;
                            let el = document.getElementById('rollSmoothBadge');
                            if (el) {
                                if (rollVar < 130) { el.className='badge badge-green';  el.innerText='CLEAN ROLL ✓'; }
                                else if (rollVar < 300) { el.className='badge badge-yellow'; el.innerText='MODERATE ROLL'; }
                                else                    { el.className='badge badge-red';    el.innerText='SLAPPING'; }
                            }
                        }
                    }
                    // Composite grounding score
                    {
                        let gs = document.getElementById('groundingScore');
                        if (gs) {
                            let scores = [
                                { id:'sdrBadge',        w:0.40 },
                                { id:'phaseDelayBadge', w:0.30 },
                                { id:'rollSmoothBadge', w:0.30 }
                            ];
                            let total = 0, wSum = 0;
                            scores.forEach(s => {
                                let el = document.getElementById(s.id);
                                if (!el) return;
                                let v = el.className.includes('badge-green') ? 1.0
                                      : el.className.includes('badge-yellow') ? 0.5
                                      : el.className.includes('badge-red')    ? 0.0
                                      : -1;
                                if (v >= 0) { total += v * s.w; wSum += s.w; }
                            });
                            if (wSum > 0) {
                                let score = Math.round((total / wSum) * 100);
                                gs.innerText = score + ' GND';
                                let gndColor = score >= 65 ? '#66bb6a' : score >= 35 ? '#ffa726' : '#ef5350';
                                gs.style.color = gndColor;
                                let gb = document.getElementById('groundingBar');
                                if (gb) { gb.style.width = score + '%'; gb.style.background = gndColor; }
                            }
                        }
                    }

                    prevAccelZLeft = aZL;
                    prevAccelZRight = aZR;
                    prevGyroPitchLeft  = gPitchL;
                    prevGyroPitchRight = gPitchR;

                    // Live debug values — update every poll when debug mode active
                    if (debugMode) {
                        let elL = document.getElementById('dbgAYL');
                        let elR = document.getElementById('dbgAYR');
                        let elP  = document.getElementById('dbgPPitch');
                        let elPY = document.getElementById('dbgPYaw');
                        if (elL) elL.innerText = aYL.toFixed(2);
                        if (elR) elR.innerText = aYR.toFixed(2);
                        if (elP)  elP.innerText  = pelvicOk ? gPitchP.toFixed(1) : '—';
                        if (elPY) elPY.innerText = pelvicOk ? gYawP.toFixed(1)   : '—';
                        // Calibration row
                        let lOff = leftMountOffset,  lTh = pitchLeftAngleRaw  - lOff;
                        let rOff = rightMountOffset, rTh = pitchRightAngleRaw - rOff;
                        let elLO = document.getElementById('dbgLOff');   if (elLO) elLO.innerText = lOff.toFixed(1);
                        let elLT = document.getElementById('dbgLTheta'); if (elLT) elLT.innerText = lTh.toFixed(1);
                        let elRO = document.getElementById('dbgROff');   if (elRO) elRO.innerText = rOff.toFixed(1);
                        let elRT = document.getElementById('dbgRTheta'); if (elRT) elRT.innerText = rTh.toFixed(1);
                        let elAL = document.getElementById('dbgAccL');   if (elAL) elAL.innerText = lastAccelAngleL.toFixed(1);
                        let elAR = document.getElementById('dbgAccR');   if (elAR) elAR.innerText = lastAccelAngleR.toFixed(1);
                        // Pelvis row — show only when sensor connected
                        let pelRow = document.getElementById('dbgPelvisRow');
                        if (pelRow) {
                            pelRow.style.display = pelvicOk ? 'block' : 'none';
                            if (pelvicOk) {
                                let ep = document.getElementById('dbgPAY'); if (ep) ep.innerText = aSagP.toFixed(2);
                                let ex = document.getElementById('dbgPAX'); if (ex) ex.innerText = aLatP.toFixed(2);
                                let ez = document.getElementById('dbgPAZ'); if (ez) ez.innerText = aVertP.toFixed(2);
                            }
                        }
                    }

                    drawPitchChart();
                    setTimeout(fetchStream, 20);
                })
                .catch(() => setTimeout(fetchStream, 100));
        }

    function drawPitchChart() {
        const midY = canvas.height / 2;
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        // Nulllinie
        ctx.strokeStyle = "rgba(255,255,255,0.15)";
        ctx.beginPath(); ctx.moveTo(0, midY); ctx.lineTo(canvas.width, midY); ctx.stroke();

        let stepX = canvas.width / maxHistory;

        // Linker Fuß (Cyan)
        ctx.strokeStyle = "#00f0ff"; ctx.lineWidth = 2; ctx.beginPath();
        for(let i=0; i<maxHistory; i++) {
            let y = midY - (pitchLeftHistory[i] / 300) * midY;
            if(i===0) ctx.moveTo(0, y); else ctx.lineTo(i * stepX, y);
        }
        ctx.stroke();

        // Rechter Fuß (Magenta)
        ctx.strokeStyle = "#ff007f"; ctx.lineWidth = 2; ctx.beginPath();
        for(let i=0; i<maxHistory; i++) {
            let y = midY - (pitchRightHistory[i] / 300) * midY;
            if(i===0) ctx.moveTo(0, y); else ctx.lineTo(i * stepX, y);
        }
        ctx.stroke();
        ctx.strokeStyle = "#ffd600"; ctx.lineWidth = 2; ctx.setLineDash([4,3]); ctx.beginPath();
        for(let i=0; i<maxHistory; i++) {
            let y = midY - (gYawHistory[i] / 80) * midY;
            if(i===0) ctx.moveTo(0, y); else ctx.lineTo(i * stepX, y);
        }
        ctx.stroke(); ctx.setLineDash([]);
    }

    fetchStream();

    // --- LEVEL SELECTOR ---
    const LEVELS      = ['beginner', 'intermediate', 'advanced'];
    const LEVEL_LABEL = { beginner: '👤 BEG', intermediate: '🏃 INT', advanced: '⭐ ADV' };
    const LEVEL_COLOR = { beginner: 'rgba(46,160,67,0.6)', intermediate: 'rgba(255,149,0,0.6)', advanced: 'rgba(139,92,246,0.6)' };

    let currentLevel = localStorage.getItem('soloLevel') || 'beginner';

    function applyLevel(level) {
        const mainEl = document.getElementById('main-layout');
        LEVELS.forEach(l => mainEl.classList.remove('level-' + l));
        mainEl.classList.add('level-' + level);
        const btn = document.getElementById('levelBtn');
        if (btn) { btn.innerText = LEVEL_LABEL[level]; btn.style.background = LEVEL_COLOR[level]; }
        localStorage.setItem('soloLevel', level);
        currentLevel = level;
    }

    function cycleLevel() {
        const next = LEVELS[(LEVELS.indexOf(currentLevel) + 1) % LEVELS.length];
        applyLevel(next);
    }

    function toggleDebug() {
        debugMode = !debugMode;
        const btn = document.getElementById('dbgBtn');
        const row = document.getElementById('debugRow');
        const card = document.getElementById('dsDebugCard');
        if (debugMode) {
            btn.style.background  = 'rgba(255,140,0,0.75)';
            btn.innerText         = '🔍 DBG ON';
            if (row)  row.style.display  = 'flex';
            if (card) card.style.display = 'block';
        } else {
            btn.style.background  = 'rgba(80,80,80,0.5)';
            btn.innerText         = '🔍 DBG';
            if (row)  row.style.display  = 'none';
            if (card) card.style.display = 'none';
        }
    }

    applyLevel(currentLevel);
</script>
</body>
</html>
)rawliteral";

#endif