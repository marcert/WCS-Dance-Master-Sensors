#ifndef SOLO_H
#define SOLO_H

const char HTML_SOLO_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>WCS Solo-Training Dashboard</title>
    <style>
                :root {
            --bg-color: #0b0e14;
            --card-bg: rgba(10, 14, 22, 0.35);
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
            padding: 12px;
            backdrop-filter: blur(2px);
            -webkit-backdrop-filter: blur(2px);
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
        }

                .card-flash {
            animation: flashCard 0.3s ease-out;
        }

        @keyframes flashCard {
            0% { border-color: rgba(255, 255, 255, 0.8); box-shadow: 0 0 12px rgba(255, 255, 255, 0.4); }
            100% { border-color: rgba(255, 255, 255, 0.1); box-shadow: none; }
        }

        /* METRICS & GAUGES */
        .metric-value { font-size: 2rem; font-weight: bold; }
        .badge { display: inline-block; padding: 2px 8px; border-radius: 12px; font-size: 0.75rem; font-weight: bold; margin-left: 6px; }
        .badge-green { background: var(--ok-color); color: #fff; }
        .badge-yellow { background: var(--warn-color); color: #000; }
        .badge-red { background: var(--danger-color); color: #fff; }

                /* CANVASES */
        canvas {
            width: 100%;
            height: 160px;
            background: rgba(0, 0, 0, 0.15);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 6px;
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
    </style>
</head>
<body>

        <video id="camera-feed" autoplay playsinline></video>

        <header>
            <h1>🕺 WCS Solo-Training Dashboard</h1>
            <div style="display: flex; gap: 8px;">
                <button id="camBtn" class="audio-toggle" style="background: rgba(0, 122, 255, 0.6);" onclick="startCamera()">📷 START CAM</button>
                <button id="flipBtn" class="audio-toggle" style="background: rgba(255, 149, 0, 0.6); display: none;" onclick="flipCamera()">🔄 FLIP</button>
                <button id="tareBtn" class="audio-toggle" style="background: rgba(0, 122, 255, 0.4);" onclick="tareFootAngles()">📐 ZERO FEET</button>
                <button id="audioBtn" class="audio-toggle" onclick="toggleAudio()">🔊 Audio Feedback: OFF</button>
            </div>
        </header>

    <!-- LIVE PITCH GRAPH -->
    <div class="card">
        <div class="card-title">Live Abrolldynamik – Pitch-Winkelgeschwindigkeit (ω_pitch)</div>
        <canvas id="chartPitch" width="800" height="200"></canvas>
    </div>

    <!-- METRICS GRID -->
    <div class="dashboard-grid">
        
                <!-- HEEL STRIKE & IMPACT JERK -->
                <div class="card" id="stepCard">
                    <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                        <div class="card-title" style="margin: 0;">Last Step (Heel/Toe-Strike & Jerk)</div>
                        <div id="debugTelemetry" style="font-family: monospace; font-size: 0.75rem; color: #58a6ff; background: rgba(0,0,0,0.4); padding: 2px 6px; border-radius: 4px;">
                            L: aY:0.0 | R: aY:0.0
                        </div>
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

                <!-- BILATERALE UBERLAPPUNG (DOUBLE STANCE) -->
                <div class="card">
                    <div class="card-title">Double Stance Overlap (Δt double_stance)</div>
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

                <!-- ASI (SYMMETRIE) & SMOOTHNESS -->
                <div class="card">
                    <div class="card-title">Roll-off Symmetry (ASI) & Smoothness</div>
                    <div style="display: flex; justify-content: space-between; align-items: center;">
                        <div>
                            <div style="font-size: 0.8rem; color:#8b949e;">Symmetry Index (ASI):</div>
                            <div class="metric-value" id="asiVal">0 %</div>
                        </div>
                        <div style="text-align: right;">
                            <div style="font-size: 0.8rem; color:#8b949e;">Roll-Smoothness:</div>
                            <div class="metric-value" id="smoothVal" style="font-size: 1.4rem;">0</div>
                        </div>
                    </div>
                </div>

    </div>

<script>
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

    const maxHistory = 200;
    let pitchLeftHistory = new Array(maxHistory).fill(0);
    let pitchRightHistory = new Array(maxHistory).fill(0);

        let prevAccelZLeft = 1.0;
        let prevAccelZRight = 1.0;
        let prevGyroPitchLeft = 0.0;
        let pitchLeftAngleRaw = 28.0;
        let pitchRightAngleRaw = 28.0;

                let lastStepTimeLeft = 0;
                let lastStepTimeRight = 0;
                let lastActiveFoot = "";

        // Montage-Nullpunkt-Offsets (Rist-Neigungskompensation)
        let leftMountOffset = 28.0;  // Standard-Sohlenneigung ca. 28 Grad
        let rightMountOffset = 28.0;

                let doubleStanceMs = 0;
        let currentStepOverlap = 0;
        let lastStepTimestamp = Date.now();
        let stepDurationMs = 500; // Standard 500ms ~= 120 BPM

                function tareFootAngles() {
            // Rist-Neigung aus den aktuellen statischen Beschleunigungswerten a_y & a_z berechnen
            leftMountOffset = pitchLeftAngleRaw;
            rightMountOffset = pitchRightAngleRaw;
        
            let btn = document.getElementById('tareBtn');
            btn.innerText = "📐 ZEROED! ✓";
            btn.style.background = "rgba(46, 160, 67, 0.8)";
            setTimeout(() => {
                btn.innerText = "📐 ZERO FEET";
                btn.style.background = "rgba(0, 122, 255, 0.4)";
            }, 1500);
        }

                let smoothnessBuffer = [];
        let smoothnessAvg = 0;

        function fetchStream() {
            fetch('/data')
                .then(res => res.json())
                .then(data => {
                    let dt = 0.02; // 20ms
                    let now = Date.now();

                                        let gPitchL = data.lG || 0;
                    let aZL = data.lA || 1.0;
                                        let aYL = data.lAy || 0.0;
                    let gPitchR = data.rG || 0;
                    let aZR = data.rA || 1.0;
                    let aYR = data.rAy || 0.0;

                    // Live Telemetrie-Debug im Header der Kachel
                    document.getElementById('debugTelemetry').innerText = `L:aY:${aYL.toFixed(2)} | R:aY:${aYR.toFixed(2)}`;

                                        // 1. Live-Pitch Kurven-Puffer
                    pitchLeftHistory.shift(); pitchLeftHistory.push(gPitchL);
                    pitchRightHistory.shift(); pitchRightHistory.push(gPitchR);

                    // Integrale für Winkelberechnung kontinuierlich aufbauen
                    pitchLeftAngleRaw += gPitchL * dt;
                    pitchRightAngleRaw += gPitchR * dt;

                    let calibratedAngleL = pitchLeftAngleRaw - leftMountOffset;
                    let calibratedAngleR = pitchRightAngleRaw - rightMountOffset;

                                        // 2. STEP & HEEL/TOE-STRIKE DETECTION (Strict L/R Alternation & Inverted Right Polarity Fix)
                                        let triggerImpact = false;
                                        let activeFoot = "";
                                        let activeTheta = 0;
                                        let activeJerk = 0;
                                        let activeDirection = "FORWARD";

                                                                                // 1. Raw Transient Step Impact Sensing
                                                                                let leftSignal = (Math.abs(aZL) > 1.08 || Math.abs(gPitchL) > 80);
                                                                                let rightSignal = (Math.abs(aZR) > 1.08 || Math.abs(gPitchR) > 80);

                                                                                let detectedFoot = null;

                                                                                if (leftSignal && rightSignal) {
                                                                                    // Pick the foot with stronger rotational motion / impact
                                                                                    detectedFoot = (Math.abs(gPitchL) >= Math.abs(gPitchR)) ? "L" : "R";
                                                                                } else if (leftSignal) {
                                                                                    detectedFoot = "L";
                                                                                } else if (rightSignal) {
                                                                                    detectedFoot = "R";
                                                                                }

                                                                                // 2. Strict Per-Foot 800 ms Lockout Guard Clause
                                                                                if (detectedFoot === "L") {
                                                                                    if (now - lastStepTimeLeft < 800) {
                                                                                        detectedFoot = null; // Suppress double trigger on Left Foot (< 800 ms)
                                                                                    }
                                                                                } else if (detectedFoot === "R") {
                                                                                    if (now - lastStepTimeRight < 800) {
                                                                                        detectedFoot = null; // Suppress double trigger on Right Foot (< 800 ms)
                                                                                    }
                                                                                }

                                                                                // 3. Process Verified Step Trigger & Polarity Logic
                                                                                if (detectedFoot === "L") {
                                                                                    // LEFT FOOT (Standard Mounting)
                                                                                    lastStepTimeLeft = now;
                                                                                    lastActiveFoot = "L";
                                                                                    triggerImpact = true;
                                                                                    activeFoot = "L";
                                                                                    activeTheta = Math.round(calibratedAngleL);
                                                                                    activeJerk = Math.abs((aZL - prevAccelZLeft) / dt);

                                                                                    let is_backward = (aYL < 0.0);   // L:aY < 0.0g  --> ⬅️ BACKWARD
                                                                                    let is_forward  = (aYL >= 0.0);  // L:aY >= 0.0g --> ➡️ FORWARD
                                                                                    activeDirection = is_backward ? "BACKWARD" : "FORWARD";
                                                                                    pitchLeftAngleRaw = leftMountOffset;
                                                                                }
                                                                                else if (detectedFoot === "R") {
                                                                                    // RIGHT FOOT (180° Inverted Hardware Mounting)
                                                                                    lastStepTimeRight = now;
                                                                                    lastActiveFoot = "R";
                                                                                    triggerImpact = true;
                                                                                    activeFoot = "R";
                                                                                    activeTheta = Math.round(calibratedAngleR);
                                                                                    activeJerk = Math.abs((aZR - prevAccelZRight) / dt);

                                                                                    let is_backward = (aYR >= 0.0);  // R:aY >= 0.0g --> ⬅️ BACKWARD (Inverted Mounting)
                                                                                    let is_forward  = (aYR < 0.0);   // R:aY < 0.0g  --> ➡️ FORWARD  (Inverted Mounting)
                                                                                    activeDirection = is_backward ? "BACKWARD" : "FORWARD";
                                                                                    pitchRightAngleRaw = rightMountOffset;
                                                                                }

                                        // Wenn ein Schritt gelandet ist -> UI & Richtungsbewertung sofort aktualisieren
                    if (triggerImpact) {
                        // Schrittdauer t_step seit letztem Schritt ermitteln
                        let currentStepDuration = Math.max(200, Math.min(1500, now - lastStepTimestamp));
                        lastStepTimestamp = now;

                        let dirBadge = document.getElementById('dirBadge');
                        let badge = document.getElementById('strikeBadge');
                        
                        document.getElementById('strikeAngleVal').innerText = Math.abs(activeTheta) + "° (" + activeFoot + ")";
                        document.getElementById('jerkVal').innerText = Math.round(activeJerk);

                                                if (activeDirection === "FORWARD") {
                            dirBadge.innerText = "➡️ FORWARD";
                            dirBadge.style.background = "#1f6beb";

                            // Forward Rating (Heel-Strike)
                            if (activeTheta >= 10 && activeTheta <= 25) {
                                badge.className = "badge badge-green"; badge.innerText = "OPTIMAL HEEL";
                            } else if (activeTheta >= 5 && activeTheta < 10) {
                                badge.className = "badge badge-yellow"; badge.innerText = "FLAT";
                            } else {
                                badge.className = "badge badge-red"; badge.innerText = "FLAT-FOOT!";
                                playImpactClick(1200);
                            }
                        } else {
                            dirBadge.innerText = "⬅️ BACKWARD";
                            dirBadge.style.background = "#a371f7";

                            // Backward Rating (Toe-Ball-Heel)
                            if (activeTheta <= 5 && activeTheta >= -20) {
                                badge.className = "badge badge-green"; badge.innerText = "OPTIMAL TOE";
                            } else if (activeTheta > 5 && activeTheta <= 10) {
                                badge.className = "badge badge-yellow"; badge.innerText = "FLAT";
                            } else {
                                badge.className = "badge badge-red"; badge.innerText = "HEEL LANDING!";
                                playImpactClick(1200); // Warning tone for heel-first backward landing
                            }
                        }

                        // Visuelles Aufblinken der Kachel bei jedem erkannten Schritt
                        let stepCard = document.getElementById('stepCard');
                        stepCard.classList.remove('card-flash');
                        void stepCard.offsetWidth; // Trigger Reflow
                        stepCard.classList.add('card-flash');

                        // Jerk Bar Visualisierung
                        let jerkPercent = Math.min(100, (activeJerk / 30) * 100);
                        document.getElementById('jerkBar').style.width = jerkPercent + "%";
                        document.getElementById('jerkBar').style.background = (activeFoot === "L") ? "var(--accent-left)" : "var(--accent-right)";

                        if (activeJerk > 20) playImpactClick(500); // Harter Aufprall Ton
                    
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

                                        // 3. Stance-Phasen & Überlappung (Sensivere Bodenkontakt-Schwellenwerte)
                    let leftOnGround = Math.abs(aZL) > 0.65;
                    let rightOnGround = Math.abs(aZR) > 0.65;

                    if (leftOnGround && rightOnGround) {
                        currentStepOverlap += dt * 1000;
                        document.getElementById('stanceBarDouble').style.width = "60%";
                    } else {
                        document.getElementById('stanceBarDouble').style.width = "0%";
                    }

                    // 4. ASI (Abroll-Symmetrie) & GEGLÄTTETE Roll-Smoothness
                    let intL = pitchLeftHistory.reduce((a, b) => a + Math.abs(b), 0);
                    let intR = pitchRightHistory.reduce((a, b) => a + Math.abs(b), 0);
                    let asi = Math.abs(1.0 - (intL / (intR || 1.0))) * 100;
                    document.getElementById('asiVal').innerText = Math.min(100, Math.round(asi)) + " %";

                    // Smoothness-Glied mit gleitendem Mittelwert (Buffer über 25 Frames / 0.5s)
                    let dOmega = (gPitchL - prevGyroPitchLeft) / dt;
                    let rawSmoothness = Math.min(100, Math.round(Math.abs(dOmega) * 0.15));
                    
                    smoothnessBuffer.push(rawSmoothness);
                    if (smoothnessBuffer.length > 25) smoothnessBuffer.shift();
                    
                    smoothnessAvg = Math.round(smoothnessBuffer.reduce((a, b) => a + b, 0) / smoothnessBuffer.length);
                    document.getElementById('smoothVal').innerText = smoothnessAvg;

                    prevAccelZLeft = aZL;
                    prevAccelZRight = aZR;
                    prevGyroPitchLeft = gPitchL;

                    drawPitchChart();
                    setTimeout(fetchStream, 20);
                })
                .catch(() => setTimeout(fetchStream, 100));
        }

    function drawPitchChart() {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        
        // Nulllinie
        ctx.strokeStyle = "rgba(255,255,255,0.15)";
        ctx.beginPath(); ctx.moveTo(0, 100); ctx.lineTo(canvas.width, 100); ctx.stroke();

        let stepX = canvas.width / maxHistory;

        // Linker Fuß (Cyan)
        ctx.strokeStyle = "#00f0ff"; ctx.lineWidth = 2; ctx.beginPath();
        for(let i=0; i<maxHistory; i++) {
            let y = 100 - (pitchLeftHistory[i] / 300) * 100;
            if(i===0) ctx.moveTo(0, y); else ctx.lineTo(i * stepX, y);
        }
        ctx.stroke();

        // Rechter Fuß (Magenta)
        ctx.strokeStyle = "#ff007f"; ctx.lineWidth = 2; ctx.beginPath();
        for(let i=0; i<maxHistory; i++) {
            let y = 100 - (pitchRightHistory[i] / 300) * 100;
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