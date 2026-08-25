#ifndef INDEX_H
#define INDEX_H

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
	<meta name="mobile-web-app-capable" content="yes">
	<meta name="apple-mobile-web-app-capable" content="yes">
	<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>WCS Ultra-Performance Dashboard</title>
    <style>
        body { 
            background: transparent; 
            color: #fff; 
            font-family: 'Arial Black', Gadget, sans-serif; 
            margin: 0; 
            padding: 10px;
            overflow: hidden; 
            box-sizing: border-box;
            height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            text-shadow: 2px 2px 4px #000; 
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

        #header-container {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 0 10px;
            height: 12vh;
        }
        
        #wert { 
            font-size: 8vh; 
            font-weight: 900; 
            line-height: 0.8; 
            margin: 0; 
            padding-bottom: 5px; 
        }
        
        .btn-group {
            display: flex;
            gap: 6px;
            margin: 0; 
            flex-wrap: wrap; 
            justify-content: flex-end;
        }

        .action-btn {
            background-color: rgba(34, 34, 34, 0.35);
            color: #fff;
            border: 1px solid rgba(255, 255, 255, 0.25);
            padding: 6px 12px;
            font-size: min(3.5vw, 14px);
            font-family: 'Arial Black', sans-serif;
            font-weight: bold;
            border-radius: 8px;
            cursor: pointer;
            backdrop-filter: blur(4px);
            -webkit-backdrop-filter: blur(4px);
        }

        #cam-btn { background-color: rgba(0, 122, 255, 0.35); }
        #flip-btn { background-color: rgba(255, 149, 0, 0.35); display: none; }
        #full-btn { background-color: rgba(80, 80, 80, 0.35); }
        #rec-btn { background-color: rgba(220, 20, 60, 0.35); }

        #freeze-btn.frozen {
            background-color: rgba(255, 59, 48, 0.8);
            animation: pulse 1.5s infinite;
        }
        
        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(255, 59, 48, 0.6); }
            70% { box-shadow: 0 0 0 12px rgba(255, 59, 48, 0); }
            100% { box-shadow: 0 0 0 0 rgba(255, 59, 48, 0); }
        }

        .graph-container { width: 100%; height: 30vh; display: flex; flex-direction: column; }
        .label { font-size: 13px; color: #ddd; margin-bottom: 2px; text-transform: uppercase; letter-spacing: 1px; }

        canvas {
            background-color: rgba(0, 0, 0, 0.4);
            border: 2px solid rgba(255, 255, 255, 0.2);
            width: 100%;
            flex: 1 1 0;
            min-height: 0;
            display: block;
            border-radius: 8px;
        }

        .legende { display: flex; gap: 15px; font-size: 11px; margin-top: 4px; color: #ddd; }
        .legende-item { display: flex; align-items: center; gap: 4px; }
        .box-left { width: 12px; height: 12px; background-color: #00ffff; border-radius: 2px; }
        .box-right { width: 12px; height: 12px; background-color: #ff00ff; border-radius: 2px; }
        .box-jerk { width: 12px; height: 12px; background-color: #ffff00; border-radius: 2px; }
        .status-bar {
            display: flex; flex-wrap: wrap; align-items: center;
            gap: 5px 10px; padding: 5px 8px;
            background: rgba(0,0,0,0.55); border-radius: 8px; min-height: 5vh;
        }
        .p-badge {
            display: inline-block; padding: 2px 8px; border-radius: 10px;
            font-size: min(3vw, 14px); font-weight: bold;
            font-family: 'Arial Black', Gadget, sans-serif;
            background: rgba(40,40,40,0.85); color: #888;
            white-space: nowrap;
        }
        .p-green  { background: rgba(15,60,15,0.9)  !important; color: #66bb6a !important; }
        .p-yellow { background: rgba(60,45,0,0.9)   !important; color: #ffa726 !important; }
        .p-red    { background: rgba(60,15,15,0.9)  !important; color: #ef5350 !important; }
        .p-blue   { background: rgba(10,20,70,0.9)  !important; color: #42a5f5 !important; }
        .p-purple { background: rgba(40,10,70,0.9)  !important; color: #ba68c8 !important; }
        #zero-btn { background-color: rgba(100,100,30,0.35); }

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

    <div id="header-container">
        <div id="wert">0 g</div>
        <div class="btn-group">
            <button id="cam-btn" class="action-btn" onclick="startCamera()">START CAM</button>
            <button id="flip-btn" class="action-btn" onclick="flipCamera()">FLIP CAM</button>
            <button id="full-btn" class="action-btn" onclick="toggleFullscreen()">FULL</button>
            <button id="freeze-btn" class="action-btn" onclick="toggleFreeze()">FREEZE</button>
            <button id="zero-btn" class="action-btn" onclick="onZeroBtn()">ZERO</button>
            <button id="p-audioBtn" class="action-btn" onclick="togglePartnerAudio()">🔇 Audio: OFF</button>
            <button id="rec-btn" class="action-btn" onclick="toggleRecording()">REC START</button>
        </div>
    </div>

    <div class="graph-container">
        <div class="label">Connection Force (-4.0 kg to +4.0 kg)</div>
        <canvas id="graph_kraft" width="1000" height="250"></canvas>
    </div>

    <div class="graph-container">
        <div class="label">Combined Analysis (Roll-off Quality, Jerk & Synchronous Error Lines)</div>
        <canvas id="graph_kombi" width="1000" height="250"></canvas>
        <div class="legende">
            <div class="legende-item"><div class="box-left"></div> Sound/Error Left</div>
            <div class="legende-item"><div class="box-right"></div> Sound/Error Right</div>
            <div class="legende-item"><div class="box-jerk"></div> Hand Jerk (Sound 1000 Hz)</div>
        </div>
    </div>

    <div class="status-bar" id="statusBar">
        <div style="width:100%;display:flex;align-items:center;gap:5px 10px;">
            <span style="font-size:min(3vw,13px);color:#aaa;">STEP:</span>
            <span id="p-dirBadge"    class="p-badge">—</span>
            <span id="p-angleVal"    style="font-size:min(3vw,13px);color:#ddd;">—</span>
            <span id="p-strikeBadge" class="p-badge" style="min-width:9em;text-align:center;">—</span>
            <span id="p-loadBadge"   class="p-badge">—</span>
        </div>
        <div id="pelvisInfoDiv" style="visibility:hidden;width:100%;display:flex;align-items:center;flex-wrap:wrap;gap:5px 10px;">
            <span style="font-size:min(2.8vw,12px);color:#ba68c8;font-weight:bold;">PELVIS:</span>
            <span id="p-hipActBadge"   class="p-badge">— HIP</span>
            <span id="p-slotBadge"     class="p-badge">— LAT</span>
            <span id="p-couplingBadge" class="p-badge">— COUP</span>
            <span id="p-bounceBadge"   class="p-badge">— BOUNCE</span>
            <span id="p-anchorBadge"   class="p-badge" style="min-width:9.5em;text-align:center;">— ANCHOR</span>
            <span id="p-hipSettleBadge" class="p-badge" style="min-width:9.5em;text-align:center;">— HIP SETTLE</span>
        </div>
    </div>

<script>
    // --- FULLSCREEN LOGIC ---
    const fullBtn = document.getElementById('full-btn');
    function toggleFullscreen() {
        if (!document.fullscreenElement && !document.webkitFullscreenElement) {
            if (document.documentElement.requestFullscreen) document.documentElement.requestFullscreen();
            else if (document.documentElement.webkitRequestFullscreen) document.documentElement.webkitRequestFullscreen();
            fullBtn.innerText = "EXIT";
        } else {
            if (document.exitFullscreen) document.exitFullscreen();
            else if (document.webkitExitFullscreen) document.webkitExitFullscreen();
            fullBtn.innerText = "FULL";
        }
    }

    // --- CAMERA LOGIC (WITH FLIP) ---
    const videoElement = document.getElementById('camera-feed');
    const camBtn = document.getElementById('cam-btn');
    const flipBtn = document.getElementById('flip-btn');

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
                flipBtn.style.display = 'block';
            } catch (error) { 
                alert("Camera access failed: " + error.message); 
            }
        }
    }

    async function flipCamera() {
        currentFacingMode = (currentFacingMode === "environment") ? "user" : "environment";
        await startCamera();
    }

    // --- RECORDING LOGIC (CANVAS CAPTURE + MIC AUDIO) ---
    let mediaRecorder;
    let recordedChunks = [];
    let audioStream = null;
    const recBtn = document.getElementById('rec-btn');

    async function toggleRecording() {
        if (mediaRecorder && mediaRecorder.state === "recording") {
            mediaRecorder.stop();
            return;
        }

        try {
            const recordCanvas = document.createElement('canvas');
            recordCanvas.width = 1000;
            recordCanvas.height = 1000;
            const recCtx = recordCanvas.getContext('2d');

            const canvasStream = recordCanvas.captureStream(30);

            try {
                if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
                    audioStream = await navigator.mediaDevices.getUserMedia({ audio: true, video: false });
                    if (audioStream && audioStream.getAudioTracks().length > 0) {
                        canvasStream.addTrack(audioStream.getAudioTracks()[0]);
                    }
                }
            } catch (audioErr) {
                console.warn("Mikrofon-Zugriff nicht möglich, nehme nur Video auf:", audioErr);
                audioStream = null;
            }

            let options = { mimeType: 'video/webm; codecs=vp9,opus' };
            if (!MediaRecorder.isTypeSupported(options.mimeType)) {
                options = { mimeType: 'video/webm' };
            }

            mediaRecorder = new MediaRecorder(canvasStream, options);

            recordedChunks = [];
            mediaRecorder.ondataavailable = function(event) {
                if (event.data && event.data.size > 0) {
                    recordedChunks.push(event.data);
                }
            };

            mediaRecorder.onstop = function() {
                if (audioStream) {
                    audioStream.getTracks().forEach(track => track.stop());
                }

                const blob = new Blob(recordedChunks, { type: 'video/webm' });
                const url = URL.createObjectURL(blob);
                
                const a = document.createElement('a');
                a.style.display = 'none';
                a.href = url;
                a.download = `WCS-Dashboard-${Date.now()}.webm`;
                document.body.appendChild(a);
                a.click();
                
                setTimeout(() => {
                    document.body.removeChild(a);
                    window.URL.revokeObjectURL(url);
                }, 100);

                recBtn.innerText = "REC START";
                recBtn.style.backgroundColor = "rgba(220, 20, 60, 0.6)";
                
                clearInterval(recInterval);
            };

            mediaRecorder.start();
            recBtn.innerText = "REC STOP";
            recBtn.style.backgroundColor = "rgba(0, 255, 0, 0.6)";

            const recInterval = setInterval(() => {
                if (mediaRecorder.state !== "recording") {
                    clearInterval(recInterval);
                    return;
                }
                
                recCtx.clearRect(0, 0, recordCanvas.width, recordCanvas.height);
                
                if (videoElement.readyState === videoElement.HAVE_ENOUGH_DATA) {
                    recCtx.drawImage(videoElement, 0, 0, recordCanvas.width, recordCanvas.height);
                } else {
                    recCtx.fillStyle = "#111";
                    recCtx.fillRect(0, 0, recordCanvas.width, recordCanvas.height);
                }

                recCtx.drawImage(canvasKraft, 0, 500, 1000, 250);
                recCtx.drawImage(canvasKombi, 0, 750, 1000, 250);

                recCtx.font = "bold 90px Arial";
                if (targetHOk) {
                    recCtx.fillStyle = targetW >= 0 ? "#00ff00" : "#ff0000";
                    recCtx.fillText(Math.round(targetW) + " g", 40, 120);
                } else {
                    recCtx.fillStyle = "#888888";
                    recCtx.fillText("- g", 40, 120);
                }

            }, 1000 / 30);

        } catch (error) {
            console.error("Recording error:", error);
            alert("Aufnahme-Fehler: " + error.message);
        }
    }

    // --- FETCH & GRAPH LOGIC ---
    const canvasKraft = document.getElementById('graph_kraft');
    const ctxKraft = canvasKraft.getContext('2d');
    const canvasKombi = document.getElementById('graph_kombi');
    const ctxKombi = canvasKombi.getContext('2d');
    const wertAnzeige = document.getElementById('wert');
    const freezeBtn = document.getElementById('freeze-btn');
    
    const intervalMs = 20;
    const totalDurationSec = 10;
    const maxPoints = (1000 / intervalMs) * totalDurationSec;
    const xStep = canvasKraft.width / maxPoints;

    // Arrays mit null initialisieren (keine gefälschten Linien beim Start)
    let kraftPoints     = new Array(maxPoints).fill(null);
    let leftFootPoints  = new Array(maxPoints).fill().map(() => ({ y: null, isError: false }));
    let rightFootPoints = new Array(maxPoints).fill().map(() => ({ y: null, isError: false }));
    let jerkPoints      = new Array(maxPoints).fill().map(() => ({ y: null, isJerkPeak: false }));

    let targetW = 0, targetLG = 0, targetLA = 1, targetRG = 0, targetRA = 1;
    let targetAx = 0, targetAy = 0, targetAz = 1;
    let targetHOk = false, targetLOk = false, targetROk = false;
    let serverError = 0;
    let serverJerk = false;

    let currentW = 0, currentLG = 0, currentLA = 1, currentRG = 0, currentRA = 1;
    let currentAx = 0.0, currentAy = 0.0, currentAz = 1.0;
    
    let prevAx = 0, prevAy = 0, prevAz = 1;
    let prevWeight = 0;

    let isFrozen = false;

    // -- new fields from /data --
    let targetLAy = 0, targetRAy = 0, targetLGr = 0, targetRGr = 0, targetLAx = 0, targetRAx = 0;
    let targetPOk = false, targetPG = 0, targetPA = 1.0, targetPAy = 0, targetPYaw = 0, targetPAx = 0;

    // -- CF filter (same init as solo.h: left aY physically inverted → rest ≈ −28°) --
    let pitchLeftAngleRaw  = -28.0, pitchRightAngleRaw =  0.0;
    let prevPitchLeftAngle = -28.0, prevPitchRightAngle = 0.0;
    let leftMountOffset    = -28.0, rightMountOffset    = 0.0;
    let lastAccelAngleL = 0, lastAccelAngleR = 0;

    // -- step detection state --
    let lastStepTimeLeft = 0, lastStepTimeRight = 0;
    let lastActiveFoot = "", stepDurationMs = 500, lastStepTimestamp = 0;
    let thetaBufferL = [], thetaBufferR = [];
    let cfWarmupFrames = 250;
    let prevAzLeft = 1.0, prevAzRight = 1.0;
    // -- delay ramp monitor (tempo-normalised weight transfer timing) --
    let delayMonActive = false, delayMonFoot = null, delayMonDir = null;
    let delayMonStartTime = 0, delayMonConsec = 0;

    // -- pelvis state (identical to solo.h) --
    let gYawAbsHistory = new Array(25).fill(0);
    let aXPHistory     = new Array(50).fill(0);
    let aZPDynHistory  = new Array(50).fill(0);
    let gYawTimedBuf   = [];
    let hipActSmoothed = 0;
    let anchorSettleActive = false, anchorSettleStartTime = 0, anchorSettleLastTrigger = 0, anchorSettleHoldUntil = 0, anchorWindowMs = 500;
    let anchorSettleSamples = { aYP: [], gYawP: [], aLatP: [] };

    // -- audio --
    let audioCtxP = null, audioEnabledP = false;
    let prevLatBadge = '', prevBncBadge = '';

    function togglePartnerAudio() {
        if (!audioCtxP) audioCtxP = new (window.AudioContext || window.webkitAudioContext)();
        audioEnabledP = !audioEnabledP;
        document.getElementById('p-audioBtn').innerText = audioEnabledP ? '🔊 Audio: ON' : '🔇 Audio: OFF';
    }

    function playPartnerBeep(freq, dur = 0.08) {
        if (!audioEnabledP || !audioCtxP) return;
        let osc = audioCtxP.createOscillator();
        let gain = audioCtxP.createGain();
        osc.type = 'sine';
        osc.frequency.setValueAtTime(freq, audioCtxP.currentTime);
        gain.gain.setValueAtTime(0.3, audioCtxP.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.01, audioCtxP.currentTime + dur);
        osc.connect(gain); gain.connect(audioCtxP.destination);
        osc.start(); osc.stop(audioCtxP.currentTime + dur);
    }

    function playDescendingSweep() {
        if (!audioEnabledP || !audioCtxP) return;
        let osc = audioCtxP.createOscillator();
        let gain = audioCtxP.createGain();
        osc.type = 'sine';
        osc.frequency.setValueAtTime(800, audioCtxP.currentTime);
        osc.frequency.linearRampToValueAtTime(350, audioCtxP.currentTime + 0.3);
        gain.gain.setValueAtTime(0.3, audioCtxP.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.01, audioCtxP.currentTime + 0.3);
        osc.connect(gain); gain.connect(audioCtxP.destination);
        osc.start(); osc.stop(audioCtxP.currentTime + 0.3);
    }

    function toggleFreeze() {
        isFrozen = !isFrozen;
        if (isFrozen) {
            freezeBtn.innerText = "FROZEN";
            freezeBtn.classList.add('frozen');
        } else {
            freezeBtn.innerText = "FREEZE";
            freezeBtn.classList.remove('frozen');
        }
    }

    function fetchSensorData() {
        if (isFrozen) {
            setTimeout(fetchSensorData, 100);
            return;
        }

        fetch('/data')
            .then(response => {
                if (!response.ok) throw new Error("HTTP-Fehler " + response.status);
                return response.json();
            })
            .then(data => {
                targetW   = data.hW;
                targetLG  = data.lG;
                targetLA  = data.lA;
                targetRG  = data.rG;
                targetRA  = data.rA;
                targetAx  = data.hAx;
                targetAy  = data.hAy;
                targetAz  = data.hAz;
                targetHOk = data.hOk;
                targetLOk = data.lOk;
                targetROk = data.rOk;
                serverError = data.err;
                serverJerk  = data.jerk;
                targetLAy  = data.lAy  ?? 0;
                targetRAy  = data.rAy  ?? 0;
                targetLGr  = data.lGr  ?? 0;
                targetRGr  = data.rGr  ?? 0;
                targetLAx  = data.lAx  ?? 0;
                targetRAx  = data.rAx  ?? 0;
                targetPOk  = data.pOk === true;
                targetPG   = data.pG   ?? 0;
                targetPA   = data.pA   ?? 1.0;
                targetPAy  = data.pAy  ?? 0;
                targetPYaw = data.pYaw ?? 0;
                targetPAx  = data.pAx  ?? 0;

                // Battery warning: blinking overlay if any active sensor ≤ 20%
                { const BATT_WARN = 20;
                  let warns = [];
                  if ((data.mBatt??0) > 0 && data.mBatt <= BATT_WARN) warns.push('Master ' + data.mBatt + '%');
                  if (targetLOk  && (data.lBatt??0) > 0 && data.lBatt <= BATT_WARN) warns.push('L ' + data.lBatt + '%');
                  if (targetROk  && (data.rBatt??0) > 0 && data.rBatt <= BATT_WARN) warns.push('R ' + data.rBatt + '%');
                  if (targetPOk  && (data.pBatt??0) > 0 && data.pBatt <= BATT_WARN) warns.push('Pelvis ' + data.pBatt + '%');
                  if (targetHOk  && (data.hBatt??0) > 0 && data.hBatt <= BATT_WARN) warns.push('Hand ' + data.hBatt + '%');
                  let wDiv = document.getElementById('battWarnDiv');
                  if (wDiv) { if (warns.length) { wDiv.innerText = '⚡ AKKU SCHWACH: ' + warns.join(' · '); wDiv.style.display = 'block'; } else { wDiv.style.display = 'none'; } }
                }

                setTimeout(fetchSensorData, intervalMs);
            })
            .catch(err => {
                console.warn("Sensor-Data fetch failed:", err);
                targetHOk = false;
                targetLOk = false;
                targetROk = false;
                setTimeout(fetchSensorData, 100);
            });
    }

    function onZeroBtn() {
        fetch('/tare').catch(() => {});
        tareFootOffsets();
    }

    function tareFootOffsets() {
        leftMountOffset  = lastAccelAngleL;
        rightMountOffset = lastAccelAngleR;
        pitchLeftAngleRaw  = leftMountOffset;
        pitchRightAngleRaw = rightMountOffset;
    }

    fetchSensorData();

    // Interpolation & Glättungs-Schleife
    setInterval(function() {
        if (isFrozen) return;

        // Anzeige Oben Links
        if (targetHOk) {
            wertAnzeige.innerText = Math.round(targetW) + " g";
            wertAnzeige.style.color = targetW >= 0 ? "#00ff00" : "#ff0000";
        } else {
            wertAnzeige.innerText = "- g";
            wertAnzeige.style.color = "#888888";
        }

        // --- HAND / WAAGE (KraftGraph) ---
        if (targetHOk) {
            currentW  += (targetW - currentW) * 0.4;
            currentAx += (targetAx - currentAx) * 0.4;
            currentAy += (targetAy - currentAy) * 0.4;
            currentAz += (targetAz - currentAz) * 0.4;

            let y_kraft = 125 - (currentW / 4000) * 125;
            y_kraft = Math.max(0, Math.min(250, y_kraft));
            kraftPoints.shift(); kraftPoints.push(y_kraft);

            // Jerk
            let dWeight = Math.abs(currentW - prevWeight);
            prevWeight = currentW;
            let dAx = currentAx - prevAx;
            let dAy = currentAy - prevAy;
            let dAz = currentAz - prevAz;
            prevAx = currentAx; prevAy = currentAy; prevAz = currentAz;

            // Express both terms as per-second rates so units are consistent before combining.
            // Previously dWeight/50 mixed grams with g*15 — that was unit-incoherent.
            let dt_s = intervalMs / 1000;
            let forceRate  = dWeight / dt_s;                                         // g/s
            let motionRate = Math.sqrt(dAx*dAx + dAy*dAy + dAz*dAz) / dt_s;        // g/s
            // Normalise: 2000 g/s force rate and 10 g/s motion rate define full scale (equal weight)
            let jerkIndex = Math.min(1.0, forceRate / 2000.0 * 0.5 + motionRate / 10.0 * 0.5);

            let y_jerk = 245 - jerkIndex * 240;
            y_jerk = Math.max(5, Math.min(245, y_jerk));
            let isJerkPeak = serverJerk; // firmware computes at native sample rate; browser duplicate was inaccurate

            jerkPoints.shift(); jerkPoints.push({ y: y_jerk, isJerkPeak: isJerkPeak });
        } else {
            kraftPoints.shift(); kraftPoints.push(null);
            jerkPoints.shift();  jerkPoints.push({ y: null, isJerkPeak: false });
        }

        // --- LINKER FUSS ---
        if (targetLOk) {
            currentLG += (targetLG - currentLG) * 0.4;
            currentLA += (targetLA - currentLA) * 0.4;

            let leftImpactDev = Math.max(0, Math.abs(currentLA) - 1.0); // only penalise impacts above 1g, not foot-lifting
            let leftQuality  = Math.abs(currentLG) / (1.0 + leftImpactDev * 2.0);
            let y_lg = 250 - (leftQuality / 300) * 250;
            y_lg = Math.max(0, Math.min(250, y_lg));
            let isLeftErr = (serverError === 1 || serverError === 3) || (Math.abs(currentLA) > 1.5 && Math.abs(currentLG) < 80.0);

            leftFootPoints.shift(); leftFootPoints.push({ y: y_lg, isError: isLeftErr });
        } else {
            leftFootPoints.shift(); leftFootPoints.push({ y: null, isError: false });
        }

        // --- RECHTER FUSS ---
        if (targetROk) {
            currentRG += (targetRG - currentRG) * 0.4;
            currentRA += (targetRA - currentRA) * 0.4;

            let rightImpactDev = Math.max(0, Math.abs(currentRA) - 1.0); // only penalise impacts above 1g, not foot-lifting
            let rightQuality = Math.abs(currentRG) / (1.0 + rightImpactDev * 2.0);
            let y_rg = 250 - (rightQuality / 300) * 250;
            y_rg = Math.max(0, Math.min(250, y_rg));
            let isRightErr = (serverError === 2 || serverError === 3) || (Math.abs(currentRA) > 1.5 && Math.abs(currentRG) < 80.0);

            rightFootPoints.shift(); rightFootPoints.push({ y: y_rg, isError: isRightErr });
        } else {
            rightFootPoints.shift(); rightFootPoints.push({ y: null, isError: false });
        }

        // === STEP DETECTION (same algorithm as solo.h) ===
        const now = Date.now();
        const dt  = intervalMs / 1000;
        let gPitchL_s = targetLG, aZL_s = targetLA, aYL_s = targetLAy;
        let gPitchR_s = targetRG, aZR_s = targetRA, aYR_s = targetRAy;

        // CF update — left aY physically inverted (same as solo.h)
        const CF_ALPHA = 0.94;
        let accelAngleL = Math.atan2(-aYL_s, aZL_s) * (180 / Math.PI);
        let accelAngleR = Math.atan2( aYR_s, aZR_s) * (180 / Math.PI);
        lastAccelAngleL = accelAngleL; lastAccelAngleR = accelAngleR;
        prevPitchLeftAngle  = pitchLeftAngleRaw;
        prevPitchRightAngle = pitchRightAngleRaw;
        if (targetLOk) pitchLeftAngleRaw  = CF_ALPHA*(pitchLeftAngleRaw  + gPitchL_s*dt) + (1-CF_ALPHA)*accelAngleL;
        if (targetROk) pitchRightAngleRaw = CF_ALPHA*(pitchRightAngleRaw + gPitchR_s*dt) + (1-CF_ALPHA)*accelAngleR;

        // Theta ring buffers (T-1 … T-8 window for ambiguous zone)
        if (thetaBufferL.length >= 10) thetaBufferL.shift();
        thetaBufferL.push(pitchLeftAngleRaw  - leftMountOffset);
        if (thetaBufferR.length >= 10) thetaBufferR.shift();
        thetaBufferR.push(pitchRightAngleRaw - rightMountOffset);
        if (cfWarmupFrames > 0) cfWarmupFrames--;

        // Step trigger (same thresholds as solo.h)
        let preJerkL_s = Math.abs(aZL_s - prevAzLeft)  / 0.005;
        let preJerkR_s = Math.abs(aZR_s - prevAzRight) / 0.005;
        let aZThr_s = stepDurationMs > 800 ? 0.92 : 0.95;
        let leftSig  = targetLOk && ((Math.abs(aZL_s) > aZThr_s && preJerkL_s > 2.0) || (Math.abs(gPitchL_s) > 80 && preJerkL_s > 8));
        let rightSig = targetROk && ((Math.abs(aZR_s) > aZThr_s && preJerkR_s > 2.0) || (Math.abs(gPitchR_s) > 80 && preJerkR_s > 8));
        if (cfWarmupFrames > 0) { leftSig = false; rightSig = false; }
        let detFoot = null;
        if      (leftSig && rightSig) detFoot = (Math.abs(aZL_s) >= Math.abs(aZR_s)) ? "L" : "R";
        else if (leftSig)  detFoot = "L";
        else if (rightSig) detFoot = "R";

        // Opposing-foot plausibility: suppress brush/scuff phantom triggers.
        if (detFoot === "L" && Math.abs(aZR_s) > 1.1 && Math.abs(aZL_s) < 0.90) detFoot = null;
        if (detFoot === "R" && Math.abs(aZL_s) > 1.1 && Math.abs(aZR_s) < 0.90) detFoot = null;

        // Lockout + alternation guard (same as solo.h)
        let lockMs = Math.max(180, Math.min(320, stepDurationMs * 0.55));
        if (detFoot === "L") {
            if (now - lastStepTimeLeft < lockMs) detFoot = null;
            else { lastStepTimeLeft = now; if (lastActiveFoot === "L") detFoot = null; }
        } else if (detFoot === "R") {
            if (now - lastStepTimeRight < lockMs) detFoot = null;
            else { lastStepTimeRight = now; if (lastActiveFoot === "R") detFoot = null; }
        }

        if (detFoot) {
            lastActiveFoot = detFoot;
            let isLeft = detFoot === "L";
            // T-1 snapshot (angle from frame before impact)
            let activeTheta = Math.round((isLeft ? prevPitchLeftAngle : prevPitchRightAngle) - (isLeft ? leftMountOffset : rightMountOffset));
            activeTheta = Math.max(-45, Math.min(45, activeTheta));
            // Step duration tracking
            let stepDur = Math.max(200, Math.min(1500, now - lastStepTimestamp));
            lastStepTimestamp = now; stepDurationMs = stepDur;
            // Direction: T-1 vs mount offset
            let activeDir = (isLeft ? prevPitchLeftAngle < leftMountOffset : prevPitchRightAngle < rightMountOffset) ? "BACKWARD" : "FORWARD";
            // Ambiguous zone override (same as solo.h: T-1 minus T-8 trend)
            if (activeTheta > 0 && activeTheta < 10) {
                let tBuf = isLeft ? thetaBufferL : thetaBufferR;
                if (tBuf.length >= 9) {
                    let trend = tBuf[tBuf.length - 2] - tBuf[tBuf.length - 9];
                    let trendThr = activeTheta > 5 ? 0.5 : 0.8;
                    if      (trend >  trendThr) activeDir = "FORWARD";
                    else if (trend < -trendThr) activeDir = "BACKWARD";
                    else                   activeDir = "AMBIGUOUS";
                } else { activeDir = "AMBIGUOUS"; }
            }
            // Hip-Foot Coupling badge (fires on each confirmed step)
            if (targetPOk && gYawTimedBuf.length >= 3) {
                let peak = gYawTimedBuf.reduce((a,b) => b.v > a.v ? b : a);
                let leadMs = now - peak.t;
                let hfEl = document.getElementById('p-couplingBadge');
                if (hfEl) {
                    if      (leadMs > 100) { hfEl.className='p-badge p-green';  hfEl.innerText='HIP LEADS'; }
                    else if (leadMs > 40)  { hfEl.className='p-badge p-yellow'; hfEl.innerText='IN SYNC'; }
                    else                   { hfEl.className='p-badge p-red';    hfEl.innerText='HIP LAGS'; }
                }
            }
            // Anchor Settle — force evaluate when backward phase ends (first non-backward step)
            if (anchorSettleActive && targetPOk && activeDir !== "BACKWARD") {
                anchorSettleLastTrigger = 0; // forces evaluation on next setInterval tick
            }

            // Anchor Settle trigger — start fresh on first backward step, extend deadline on each subsequent one
            if (targetPOk && activeDir === "BACKWARD" && now >= anchorSettleHoldUntil) {
                if (!anchorSettleActive) {
                    anchorSettleActive = true; anchorSettleStartTime = now;
                    anchorWindowMs = Math.min(500, Math.max(280, stepDurationMs));
                    anchorSettleSamples = { aYP: [], gYawP: [], aLatP: [] };
                }
                anchorSettleLastTrigger = now;
                let ab = document.getElementById('p-anchorBadge');
                if (ab) { ab.className='p-badge'; ab.innerText='MEASURING...'; }
                let hse0 = document.getElementById('p-hipSettleBadge');
                if (hse0) { hse0.className='p-badge'; hse0.style.cssText='background:#1e272e;color:#8b949e;'; hse0.innerText='MEASURING...'; }
            }
            // Capture jerk at trigger moment (same scaling as solo.h)
            let activeJerk_p = isLeft ? preJerkL_s : preJerkR_s;
            // Direction badge: reliable only at θ-zone extremes
            let dirEl = document.getElementById('p-dirBadge');
            let angEl = document.getElementById('p-angleVal');
            let strEl = document.getElementById('p-strikeBadge');
            let fStr = isLeft ? ' L' : ' R';
            if (dirEl) {
                if      (activeTheta >= 8)  { dirEl.className='p-badge p-blue';   dirEl.innerText='➡ FWD'+fStr; }
                else if (activeTheta < -8)  { dirEl.className='p-badge p-purple'; dirEl.innerText='⬅ BACK'+fStr; }
                else                        { dirEl.className='p-badge';          dirEl.innerText='—'+fStr; }
            }
            if (angEl) angEl.innerText = activeTheta + '°';
            // Strike badge: landing quality, direction-agnostic
            if (strEl) {
                if (activeTheta >= 8) {
                    if (activeJerk_p > 88) { strEl.className='p-badge p-red';    strEl.innerText='HEEL SLAM ⚠'; playPartnerBeep(1200); }
                    else                   { strEl.className='p-badge p-green';  strEl.innerText='HEEL STRIKE ✓'; }
                } else if (activeTheta < -8) {
                    if (activeJerk_p > 88) { strEl.className='p-badge p-red';    strEl.innerText='TOE JAM ⚠'; playPartnerBeep(1200); }
                    else                   { strEl.className='p-badge p-green';  strEl.innerText='TOE-FIRST ✓'; }
                } else {
                    if      (activeJerk_p > 88)  { strEl.className='p-badge p-red';    strEl.innerText='HARD IMPACT ⚠'; playPartnerBeep(1200); }
                    else if (activeJerk_p > 80)  { strEl.className='p-badge p-yellow'; strEl.innerText='MODERATE'; }
                    else                         { strEl.className='p-badge p-green';  strEl.innerText='SOFT ✓'; }
                }
            }
            // Start delay ramp monitor
            delayMonActive = true; delayMonFoot = detFoot; delayMonDir = activeDir;
            delayMonStartTime = now; delayMonConsec = 0;
            let dlEl = document.getElementById('p-loadBadge');
            if (dlEl) { dlEl.className='p-badge'; dlEl.innerText='…'; }
        }
        prevAzLeft = aZL_s; prevAzRight = aZR_s;

        // === DELAY RAMP MONITOR — tempo-normalised weight transfer timing ===
        if (delayMonActive) {
            let monAz = delayMonFoot === "L" ? aZL_s : aZR_s;
            let monGy = delayMonFoot === "L" ? gPitchL_s : gPitchR_s;
            if (Math.abs(monAz - 1.0) < 0.08 && Math.abs(monGy) < 15) {
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
                let dlEl = document.getElementById('p-loadBadge');
                if (dlEl) {
                    if      (ratio < quickThr) { dlEl.className='p-badge p-yellow'; dlEl.innerText='QUICK'; }
                    else if (ratio <= lateThr) { dlEl.className='p-badge p-green';  dlEl.innerText='DELAYED ✓'; }
                    else                       { dlEl.className='p-badge p-yellow'; dlEl.innerText='LATE'; }
                }
            }
        }

        // === PELVIS METRICS (all 5 — same thresholds as solo.h, no level gating) ===
        let gYawP_p = targetPYaw, aZP_p = -targetPAy, aYP_p = targetPAx, aXP_p = targetPA;
        if (targetPOk) {
            document.getElementById('pelvisInfoDiv').style.visibility = 'visible';
            // Hip Activation — rolling max of |gYawP| over 500ms, IIR-smoothed
            gYawAbsHistory.shift(); gYawAbsHistory.push(Math.abs(gYawP_p));
            let gYawPeak = Math.max(...gYawAbsHistory);
            hipActSmoothed = hipActSmoothed * 0.9 + gYawPeak * 0.1;
            let hipEl = document.getElementById('p-hipActBadge');
            if (hipEl) {
                let hipThrActive = Math.round(60 * 500 / Math.max(400, stepDurationMs));
                let hipThrMod    = Math.round(25 * 500 / Math.max(400, stepDurationMs));
                if      (hipActSmoothed >= hipThrActive) { hipEl.className='p-badge p-green';  hipEl.innerText='🌀 ACTIVE'; }
                else if (hipActSmoothed >= hipThrMod)    { hipEl.className='p-badge p-yellow'; hipEl.innerText='MODERATE'; }
                else                                     { hipEl.className='p-badge p-red';    hipEl.innerText='STIFF HIPS'; }
            }
            // Lateral Stability — variance of aXP over 1s
            aXPHistory.shift(); aXPHistory.push(aXP_p);
            let aXMean = aXPHistory.reduce((a,b)=>a+b,0) / aXPHistory.length;
            let aXVar  = aXPHistory.reduce((a,b)=>a+(b-aXMean)**2,0) / aXPHistory.length;
            let latEl = document.getElementById('p-slotBadge');
            if (latEl) {
                if      (aXVar < 0.004) { latEl.className='p-badge p-green';  latEl.innerText='STABLE'; }
                else if (aXVar < 0.015) { latEl.className='p-badge p-yellow'; latEl.innerText='SLIGHT SWAY'; }
                else                    { latEl.className='p-badge p-red';    latEl.innerText='LATERAL SWAY';
                    if (prevLatBadge !== 'LATERAL SWAY') playPartnerBeep(400, 0.25);
                }
                prevLatBadge = latEl.innerText;
            }
            // Vertical Bounce — variance of dynamic aZP (gravity removed) over 1s
            aZPDynHistory.shift(); aZPDynHistory.push(aZP_p - 1.0);
            let aZMean = aZPDynHistory.reduce((a,b)=>a+b,0) / aZPDynHistory.length;
            let aZVar  = aZPDynHistory.reduce((a,b)=>a+(b-aZMean)**2,0) / aZPDynHistory.length;
            let bncEl = document.getElementById('p-bounceBadge');
            if (bncEl) {
                if      (aZVar < 0.006) { bncEl.className='p-badge p-green';  bncEl.innerText='GROUNDED'; }
                else if (aZVar < 0.020) { bncEl.className='p-badge p-yellow'; bncEl.innerText='SLIGHT BOUNCE'; }
                else                    { bncEl.className='p-badge p-red';    bncEl.innerText='BOUNCY';
                    if (prevBncBadge !== 'BOUNCY') { playPartnerBeep(600, 0.08); setTimeout(()=>playPartnerBeep(600, 0.08), 130); }
                }
                prevBncBadge = bncEl.innerText;
            }
            // gYaw timed ring — {t, v} pairs, 600ms window (for hip-foot coupling)
            gYawTimedBuf.push({ t: now, v: Math.abs(gYawP_p) });
            while (gYawTimedBuf.length > 0 && now - gYawTimedBuf[0].t > 600) gYawTimedBuf.shift();
            // Anchor Settle — collect tempo-adaptive window, evaluate at end
            if (anchorSettleActive) {
                anchorSettleSamples.aYP.push(aYP_p);
                anchorSettleSamples.gYawP.push(Math.abs(gYawP_p));
                anchorSettleSamples.aLatP.push(aXP_p);
                if (now - anchorSettleLastTrigger >= anchorWindowMs) {
                    anchorSettleActive = false;
                    let n = anchorSettleSamples.aYP.length;
                    if (n >= 8) {
                        let half = Math.floor(n/2);
                        let earlyAY  = anchorSettleSamples.aYP.slice(0,half),  lateAY  = anchorSettleSamples.aYP.slice(half);
                        let earlyYaw = anchorSettleSamples.gYawP.slice(0,half), lateYaw = anchorSettleSamples.gYawP.slice(half);
                        let earlyAYMag = earlyAY.reduce((a,b)=>a+Math.abs(b),0)/earlyAY.length;
                        let lateAYMag  = lateAY.reduce((a,b)=>a+Math.abs(b),0)/lateAY.length;
                        let earlyYawM  = earlyYaw.reduce((a,b)=>a+b,0)/earlyYaw.length;
                        let lateYawM   = lateYaw.reduce((a,b)=>a+b,0)/lateYaw.length;
                        let decelScore   = Math.min(1,Math.max(0,(earlyAYMag-lateAYMag+0.05)/0.25));
                        let yawDampScore = Math.min(1,Math.max(0,(earlyYawM-lateYawM)/25));
                        let lateYawVar   = lateYaw.reduce((a,b)=>a+(b-lateYawM)**2,0)/lateYaw.length;
                        let stabilScore  = Math.max(0, 1-lateYawVar/400);
                        let score = Math.round((decelScore*0.35+yawDampScore*0.35+stabilScore*0.30)*100);
                        let ab = document.getElementById('p-anchorBadge');
                        if (ab) {
                            if      (score >= 60) { ab.className='p-badge p-green';  ab.innerText='ANCHORED ('+score+')'; }
                            else if (score >= 30) { ab.className='p-badge p-yellow'; ab.innerText='SETTLING ('+score+')'; }
                            else                  { ab.className='p-badge p-red';    ab.innerText='UNSTABLE ('+score+')'; playDescendingSweep(); }
                        }
                        // Hip Settle — lateral pelvic impulse in first half of window
                        let earlyLat    = anchorSettleSamples.aLatP.slice(0, half);
                        let lateLat     = anchorSettleSamples.aLatP.slice(half);
                        let earlyLatPeak = Math.max(...earlyLat.map(v => Math.abs(v)));
                        let lateLatMean  = lateLat.reduce((a,b) => a+b, 0) / lateLat.length;
                        let lateLatVar   = lateLat.reduce((a,b) => a + (b - lateLatMean)**2, 0) / lateLat.length;
                        let hse = document.getElementById('p-hipSettleBadge');
                        if (hse) {
                            hse.style.cssText = '';
                            if      (earlyLatPeak > 0.30)                           { hse.className='p-badge p-yellow'; hse.innerText='OVERSWING ⚠'; }
                            else if (earlyLatPeak > 0.10 && lateLatVar < 0.015)     { hse.className='p-badge p-green';  hse.innerText='HIP SETTLE ✓'; }
                            else if (earlyLatPeak > 0.05)                           { hse.className='p-badge p-yellow'; hse.innerText='SLIGHT SETTLE'; }
                            else                                                     { hse.className='p-badge p-red';    hse.innerText='NO HIP SETTLE'; }
                        }
                        anchorSettleHoldUntil = now + 2000;
                    }
                }
            }
        } else {
            document.getElementById('pelvisInfoDiv').style.visibility = 'hidden';
            anchorSettleActive = false;
        }

    }, intervalMs); 

    function draw() {
        if (!isFrozen) {
            drawKraftGraph();
            drawKombiGraph();
        }
        requestAnimationFrame(draw);
    }
    requestAnimationFrame(draw);

    function drawKraftGraph() {
        ctxKraft.clearRect(0, 0, canvasKraft.width, canvasKraft.height);
        
        // Nulllinie
        ctxKraft.strokeStyle = 'rgba(255, 255, 255, 0.2)'; 
        ctxKraft.lineWidth = 2;
        ctxKraft.beginPath(); ctxKraft.moveTo(0, 125); ctxKraft.lineTo(canvasKraft.width, 125); ctxKraft.stroke();

        ctxKraft.lineWidth = 6;
        let drawing = false;
        let isGreen = true;

        for (let i = 0; i < kraftPoints.length; i++) {
            let val = kraftPoints[i];
            let x = i * xStep;

            if (val === null) {
                if (drawing) {
                    ctxKraft.stroke();
                    drawing = false;
                }
                continue;
            }

            let nextIsGreen = (val < 125);

            if (!drawing) {
                ctxKraft.beginPath();
                ctxKraft.strokeStyle = nextIsGreen ? '#00ff00' : '#ff0000';
                ctxKraft.moveTo(x, val);
                isGreen = nextIsGreen;
                drawing = true;
            } else {
                if (nextIsGreen !== isGreen) {
                    ctxKraft.lineTo(x, val);
                    ctxKraft.stroke();
                    ctxKraft.beginPath();
                    ctxKraft.strokeStyle = nextIsGreen ? '#00ff00' : '#ff0000';
                    ctxKraft.moveTo(x, val);
                    isGreen = nextIsGreen;
                } else {
                    ctxKraft.lineTo(x, val);
                }
            }
        }
        if (drawing) ctxKraft.stroke();
    }

    function drawKombiGraph() {
        ctxKombi.clearRect(0, 0, canvasKombi.width, canvasKombi.height);
        
        // Nulllinie
        ctxKombi.strokeStyle = 'rgba(255, 255, 255, 0.2)';
        ctxKombi.lineWidth = 2;
        ctxKombi.beginPath(); ctxKombi.moveTo(0, 125); ctxKombi.lineTo(canvasKombi.width, 125); ctxKombi.stroke();

        // Fehler-Markierungen (vertikal)
        for (let i = 0; i < maxPoints; i++) {
            let x = i * xStep;
            let leftErr  = leftFootPoints[i].isError;
            let rightErr = rightFootPoints[i].isError;
            let jerkPeak = jerkPoints[i].isJerkPeak;

            if (leftErr || rightErr || jerkPeak) {
                ctxKombi.beginPath();
                ctxKombi.setLineDash([]); 

                if (leftErr && rightErr) {
                    ctxKombi.strokeStyle = '#ff0000'; 
                    ctxKombi.lineWidth = 3;
                } else if (leftErr) {
                    ctxKombi.strokeStyle = 'rgba(0, 255, 255, 0.9)'; 
                    ctxKombi.lineWidth = 2;
                } else if (rightErr) {
                    ctxKombi.strokeStyle = 'rgba(255, 0, 255, 0.9)'; 
                    ctxKombi.lineWidth = 2;
                } else if (jerkPeak) {
                    ctxKombi.strokeStyle = 'rgba(255, 255, 0, 0.6)'; 
                    ctxKombi.setLineDash([4, 4]);
                    ctxKombi.lineWidth = 2;
                }

                ctxKombi.moveTo(x, 0);
                ctxKombi.lineTo(x, canvasKombi.height);
                ctxKombi.stroke();
                ctxKombi.setLineDash([]); 
            }
        }

        // Linien zeichnen mit Unterbrechung bei offline (null)
        drawSeries(ctxKombi, leftFootPoints, '#00ffff', 4);  // Links (Cyan)
        drawSeries(ctxKombi, rightFootPoints, '#ff00ff', 4); // Rechts (Magenta)
        drawSeries(ctxKombi, jerkPoints, '#ffff00', 3);      // Jerk (Gelb)
    }

    // Hilfsfunktion zum Zeichnen von Punktfolgen mit Unterbrechungen
    function drawSeries(ctx, points, color, width) {
        ctx.lineWidth = width;
        ctx.strokeStyle = color;
        let drawing = false;

        for (let i = 0; i < points.length; i++) {
            let pt = points[i];
            let x = i * xStep;

            if (pt.y === null) {
                if (drawing) {
                    ctx.stroke();
                    drawing = false;
                }
                continue;
            }

            if (!drawing) {
                ctx.beginPath();
                ctx.moveTo(x, pt.y);
                drawing = true;
            } else {
                ctx.lineTo(x, pt.y);
            }
        }
        if (drawing) ctx.stroke();
    }
</script>
</body>
</html>
)rawliteral";

#endif