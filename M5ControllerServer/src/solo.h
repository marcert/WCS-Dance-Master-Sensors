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
        .badge { display: inline-block; padding: 2px 8px; border-radius: 12px; font-size: 0.75rem; font-weight: bold; margin-left: 6px; text-shadow: 0 1px 3px rgba(0,0,0,0.9); }
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

        .bar-container { height: 16px; background: rgba(255,255,255,0.1); border-radius: 8px; overflow: hidden; margin-top: 6px; }
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
        }

        /* ====================================================
           LANDSCAPE — 2-column split, no vertical scroll
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

            /* Left col (graph) + right col (metric cards) side by side */
            #main-layout {
                flex-direction: row;
                gap: 8px;
                overflow: hidden;
            }

            /* Left column: graph fills available height */
            #graphCard {
                flex: 0 0 47%;
                overflow: hidden;
            }

            /* Canvas wrapper grows to fill the graphCard */
            #canvas-wrapper {
                flex: 1;
                height: auto;
                min-height: 0;
            }

            /* Right column: 2×2 grid of metric cards */
            .dashboard-grid {
                flex: 1;
                grid-template-columns: 1fr 1fr;
                gap: 6px;
                align-content: start;
                overflow: hidden;
                min-height: 0;
            }

            .card { padding: 5px 8px; }
            .metric-value { font-size: 1.4rem; }
            .stance-timeline { height: 22px; margin-top: 4px; }
            .bar-container { margin-top: 4px; }
        }
    </style>
</head>
<body>

        <video id="camera-feed" autoplay playsinline></video>

                <header>
            <h1>🕺 WCS Solo-Training Dashboard</h1>
            <div style="display: flex; gap: 8px;">
                <button id="camBtn" class="audio-toggle" style="background: rgba(0, 122, 255, 0.6);" onclick="startCamera()">📷 CAM</button>
                <button id="flipBtn" class="audio-toggle" style="background: rgba(255, 149, 0, 0.6); display: none;" onclick="flipCamera()">🔄 FLIP</button>
                <button id="fullBtn" class="audio-toggle" style="background: rgba(80, 80, 80, 0.6);" onclick="toggleFullscreen()">⛶ FULL</button>
                <button id="tareBtn" class="audio-toggle" style="background: rgba(0, 122, 255, 0.4);" onclick="tareFootAngles()">📐 ZERO</button>
                <button id="audioBtn" class="audio-toggle" onclick="toggleAudio()">🔇 Biofeedback: OFF</button>
            </div>
        </header>

<main id="main-layout">

        <!-- LIVE PITCH GRAPH -->
    <div class="card" id="graphCard">
        <div class="card-title">Live Roll-off Dynamics – Pitch Angular Velocity (&omega;_pitch)</div>
        <div id="canvas-wrapper"><canvas id="chartPitch"></canvas></div>
    </div>

    <!-- METRICS GRID -->
    <div class="dashboard-grid">
        
                                <!-- HEEL STRIKE & IMPACT JERK -->
                <div class="card" id="stepCard">
                    <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                        <div class="card-title" style="margin: 0;">Last Step (Heel/Toe-Strike & Jerk)</div>
                    </div>
                    <div style="display: flex; justify-content: space-between; align-items: baseline;">
                        <div>
                            <span id="dirBadge" class="badge" style="background:#30363d; font-size:0.85rem;">➡️ FORWARD</span>
                            <span id="strikeAngleVal" class="metric-value">0°</span>
                            <span id="strikeBadge" class="badge badge-green">OPTIMAL</span>
                        </div>
                        <div style="text-align: right; font-size: 0.9rem; color: #8b949e;">
                            Impact Jerk: <strong id="jerkVal" style="color:#fff;">0</strong> g/s
                        </div>
                    </div>
                    <div class="bar-container">
                        <div id="jerkBar" class="bar-fill" style="background: var(--accent-left);"></div>
                    </div>
                </div>

                                <!-- BILATERAL OVERLAP (DOUBLE STANCE) -->
                <div class="card">
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
                </div>

                                <!-- ASI (SYMMETRY) & SMOOTHNESS -->
                <div class="card">
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

    const maxHistory = 200;
    let pitchLeftHistory = new Array(maxHistory).fill(0);
    let pitchRightHistory = new Array(maxHistory).fill(0);

        let prevAccelZLeft = 1.0;
        let prevAccelZRight = 1.0;
        let prevGyroPitchLeft = 0.0;
        let prevGyroPitchRight = 0.0;
        let pitchLeftAngleRaw  = -28.0; // Left sensor aY is physically inverted: rest angle = atan2(-aYL, aZL) ≈ -28°
        let pitchRightAngleRaw =   0.0; // Right sensor rests at ~0° with atan2(aY, aZ) formula

                let lastStepTimeLeft = 0;
                let lastStepTimeRight = 0;
                let lastActiveFoot = "";
                let prevPitchLeftAngle  = -28.0; // T-1 CF angle saved before each frame's CF update
                let prevPitchRightAngle =   0.0;

                // Instep pitch offsets
        let leftMountOffset  = -28.0; // Left sensor: aY inverted, rest angle ≈ -28° with atan2(-aYL, aZL)
        let rightMountOffset =   0.0; // Right sensor neutral = 0° with atan2(aYR, aZR)

                let doubleStanceMs = 0;
        let currentStepOverlap = 0;
        let lastStepTimestamp = Date.now();
        let stepDurationMs = 500; // Standard 500ms ~= 120 BPM

                function tareFootAngles() {
                    // Use the live accel-snapshot angle (not the CF angle) so the offset is
                    // immune to gyro drift that may have accumulated during active movement.
                    leftMountOffset  = lastAccelAngleL;
                    rightMountOffset = lastAccelAngleR;
                    // Sync CF integration to the same reference so the graph re-centres too.
                    pitchLeftAngleRaw  = leftMountOffset;
                    pitchRightAngleRaw = rightMountOffset;
        
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
        let stanceBuffer = new Array(100).fill(0); // rolling 2-second window: 0=none,1=left,2=right,3=both
        let asiSmoothed = 0; // IIR-smoothed ASI to suppress single-swing asymmetry spikes

        // Last accel-snapshot angles — updated every poll cycle, used by tare instead of CF angle
        let lastAccelAngleL = 0;
        let lastAccelAngleR = 0;

        function fetchStream() {
            fetch('/data')
                .then(res => res.json())
                .then(data => {
                    let dt = 0.02; // 20ms
                    let now = Date.now();

                                        let gPitchL = data.lG ?? 0;
                    let aZL = data.lA ?? 1.0;
                    let aYL = data.lAy ?? 0.0;
                    let gPitchR = data.rG ?? 0;
                    let aZR = data.rA ?? 1.0;
                    let aYR = data.rAy ?? 0.0;
                    let leftOk  = data.lOk === true;
                    let rightOk = data.rOk === true;

                                        // 1. Live Pitch Curve Buffer — low-pass filtered (α=0.25) to suppress vibration noise
                    const LP_ALPHA = 0.25;
                    let filtL = pitchLeftHistory[pitchLeftHistory.length  - 1] * (1 - LP_ALPHA) + gPitchL * LP_ALPHA;
                    let filtR = pitchRightHistory[pitchRightHistory.length - 1] * (1 - LP_ALPHA) + gPitchR * LP_ALPHA;
                    pitchLeftHistory.shift();  pitchLeftHistory.push(filtL);
                    pitchRightHistory.shift(); pitchRightHistory.push(filtR);

                    // Complementary filter: gyro integration for short-term dynamics,
                    // accel angle for long-term drift correction (2% per frame at 50 Hz ≈ 1°/s max correction)
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
                                                                                let leftSignal  = leftOk  && (Math.abs(aZL) > 1.08 || (Math.abs(gPitchL) > 80 && preJerkL > 8));
                                                                                let rightSignal = rightOk && (Math.abs(aZR) > 1.08 || (Math.abs(gPitchR) > 80 && preJerkR > 8));

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

                                                                                                                                                                // 2. Per-Foot 220 ms Lockout + Alternation Guard
                                                                                // Feet must alternate (L→R→L or R→L→R). Same foot firing twice without
                                                                                // the other foot in between = liftoff re-detection or vibration ghost.
                                                                                if (detectedFoot === "L") {
                                                                                    if (now - lastStepTimeLeft < 220) {
                                                                                        detectedFoot = null; // hard lockout: too soon after last L event
                                                                                    } else {
                                                                                        lastStepTimeLeft = now;
                                                                                        if (lastActiveFoot === "L") {
                                                                                            detectedFoot = null; // alternation guard: L→L without R in between
                                                                                        }
                                                                                    }
                                                                                } else if (detectedFoot === "R") {
                                                                                    if (now - lastStepTimeRight < 220) {
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
                                                                                    pitchLeftAngleRaw = leftMountOffset;
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
                                                                                    pitchRightAngleRaw = rightMountOffset;
                                                                                }

                                        // Wenn ein Schritt gelandet ist -> UI & Richtungsbewertung sofort aktualisieren
                    if (triggerImpact) {
                                                // Determine step duration t_step since last step
                        let currentStepDuration = Math.max(200, Math.min(1500, now - lastStepTimestamp));
                        lastStepTimestamp = now;

                        let dirBadge = document.getElementById('dirBadge');
                        let badge = document.getElementById('strikeBadge');
                        
                        // Guard against accel transients during rapid direction changes (e.g. 107° spike in v4 analysis).
                        // No valid WCS step angle exceeds ±45° — values outside that range are sensor artefacts.
                        activeTheta = Math.max(-45, Math.min(45, activeTheta));

                        // Flat contact: |θ| < 5° is at the FORWARD/BACKWARD aY boundary — direction unreliable.
                        if (Math.abs(activeTheta) < 5) activeDirection = "AMBIGUOUS";

                        document.getElementById('strikeAngleVal').innerText = activeTheta + "° (" + activeFoot + ")";
                        document.getElementById('jerkVal').innerText = Math.round(activeJerk / 4); // ÷4 converts internal 200Hz-scaled value to actual g/s at poll rate

                                                if (activeDirection === "FORWARD") {
                            dirBadge.innerText = "➡️ FORWARD";
                            dirBadge.style.background = "#1f6beb";

                            // Forward Rating (Heel-Strike)
                            if (activeTheta > 35) {
                                badge.className = "badge badge-yellow"; badge.innerText = "HEEL SPIKE";  // gyro overshoot during fast swing
                            } else if (activeTheta >= 10) {
                                badge.className = "badge badge-green"; badge.innerText = "OPTIMAL HEEL";
                            } else if (activeTheta >= 5) {
                                badge.className = "badge badge-yellow"; badge.innerText = "FLAT";
                            } else {
                                badge.className = "badge badge-red"; badge.innerText = "FLAT-FOOT!";
                                if (activeJerk > 160) playImpactClick(1200); // only alert on hard flat impacts (>40 g/s displayed)
                            }
                        } else if (activeDirection === "BACKWARD") {
                            dirBadge.innerText = "⬅️ BACKWARD";
                            dirBadge.style.background = "#a371f7";

                            // Backward Rating (Toe-Ball-Heel)
                            if (activeTheta >= 10) {
                                badge.className = "badge badge-red"; badge.innerText = "HEEL LANDING!";
                                playImpactClick(1200);
                            } else if (activeTheta > 5) {
                                badge.className = "badge badge-yellow"; badge.innerText = "HEEL DROP";
                            } else if (activeTheta >= -20) {
                                badge.className = "badge badge-green"; badge.innerText = "OPTIMAL TOE";
                            } else {
                                badge.className = "badge badge-yellow"; badge.innerText = "HEEL SPIKE";  // extreme backward overshoot
                            }
                        } else {
                            // AMBIGUOUS: |θ| < 5° — foot landed flat, aY unreliable for direction.
                            dirBadge.innerText = "↔️ FLAT";
                            dirBadge.style.background = "#555";
                            badge.className = "badge badge-red"; badge.innerText = "FLAT-FOOT!";
                            if (activeJerk > 160) playImpactClick(1200);
                        }

                        // Visuelles Aufblinken der Kachel bei jedem erkannten Schritt
                        let stepCard = document.getElementById('stepCard');
                        stepCard.classList.remove('card-flash');
                        void stepCard.offsetWidth; // Trigger Reflow
                        stepCard.classList.add('card-flash');

                        // Jerk Bar Visualisierung
                        let jerkPercent = Math.min(100, (activeJerk / 120) * 100); // bar scaled to native 200 Hz dt
                        document.getElementById('jerkBar').style.width = jerkPercent + "%";
                        document.getElementById('jerkBar').style.background = (activeFoot === "L") ? "var(--accent-left)" : "var(--accent-right)";

                        if (activeJerk > 120) playImpactClick(500); // threshold scaled to native 200 Hz dt (×4 vs 20 ms poll)
                    
                        // Dynamic Tempo-Adaptive Doppelstand-Ratio (%)
                        let stanceRatio = Math.round((currentStepOverlap / currentStepDuration) * 100);
                        document.getElementById('doubleStanceVal').innerText = Math.round(currentStepOverlap) + " ms";
                        document.getElementById('stanceRatioVal').innerText = "(" + stanceRatio + "%)";

                                                let stanceBadge = document.getElementById('stanceBadge');
                        if (stanceRatio >= 18 && stanceRatio <= 38) {
                            stanceBadge.className = "badge badge-green"; stanceBadge.innerText = "OPTIMAL ROLL";
                        } else if (stanceRatio < 18) {
                            stanceBadge.className = "badge badge-yellow"; stanceBadge.innerText = "HECTIC";
                        } else {
                            stanceBadge.className = "badge badge-yellow"; stanceBadge.innerText = "SLUGGISH";
                        }

                        currentStepOverlap = 0; // Reset für neuen Schritt
                    }

                    // 3. Stance-Phasen & Überlappung — offline sensors never count as grounded
                    let leftOnGround  = leftOk  && Math.abs(aZL) > 0.55;
                    let rightOnGround = rightOk && Math.abs(aZR) > 0.55;

                    if (leftOnGround && rightOnGround) {
                        currentStepOverlap += dt * 1000;
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
                            if (asiRounded <= 10)      { asiBadge.className = "badge badge-green";  asiBadge.innerText = "SYMMETRIC"; }
                            else if (asiRounded <= 25) { asiBadge.className = "badge badge-yellow"; asiBadge.innerText = "MINOR ASYM"; }
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

                    prevAccelZLeft = aZL;
                    prevAccelZRight = aZR;
                    prevGyroPitchLeft  = gPitchL;
                    prevGyroPitchRight = gPitchR;

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
    }

    fetchStream();
</script>
</body>
</html>
)rawliteral";

#endif