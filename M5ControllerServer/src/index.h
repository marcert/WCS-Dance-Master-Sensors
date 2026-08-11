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
            background-color: rgba(34, 34, 34, 0.6); 
            color: #fff;
            border: 2px solid rgba(255, 255, 255, 0.3);
            padding: 6px 10px;
            font-size: 3vw;
            font-family: 'Arial Black', sans-serif;
            font-weight: bold;
            border-radius: 8px;
            cursor: pointer;
            backdrop-filter: blur(4px);
            -webkit-backdrop-filter: blur(4px); 
        }
        
        #cam-btn { background-color: rgba(0, 122, 255, 0.6); }
        #flip-btn { background-color: rgba(255, 149, 0, 0.6); display: none; }
        #full-btn { background-color: rgba(80, 80, 80, 0.6); }
        #rec-btn { background-color: rgba(220, 20, 60, 0.6); }

        #freeze-btn.frozen {
            background-color: rgba(255, 59, 48, 0.8);
            animation: pulse 1.5s infinite;
        }
        
        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(255, 59, 48, 0.6); }
            70% { box-shadow: 0 0 0 12px rgba(255, 59, 48, 0); }
            100% { box-shadow: 0 0 0 0 rgba(255, 59, 48, 0); }
        }

        .graph-container { width: 100%; height: 36vh; display: flex; flex-direction: column; }
        .label { font-size: 13px; color: #ddd; margin-bottom: 2px; text-transform: uppercase; letter-spacing: 1px; }
        
        canvas { 
            background-color: rgba(0, 0, 0, 0.4); 
            border: 2px solid rgba(255, 255, 255, 0.2); 
            width: 100%; 
            height: 100%; 
            display: block; 
            border-radius: 8px; 
        }
        
        .legende { display: flex; gap: 15px; font-size: 11px; margin-top: 4px; color: #ddd; }
        .legende-item { display: flex; align-items: center; gap: 4px; }
        .box-left { width: 12px; height: 12px; background-color: #00ffff; border-radius: 2px; }
        .box-right { width: 12px; height: 12px; background-color: #ff00ff; border-radius: 2px; }
        .box-jerk { width: 12px; height: 12px; background-color: #ffff00; border-radius: 2px; }
    </style>
</head>
<body>

    <video id="camera-feed" autoplay playsinline></video>

    <div id="header-container">
        <div id="wert">0 g</div>
        <div class="btn-group">
            <button id="cam-btn" class="action-btn" onclick="startCamera()">START CAM</button>
            <button id="flip-btn" class="action-btn" onclick="flipCamera()">FLIP CAM</button>
            <button id="full-btn" class="action-btn" onclick="toggleFullscreen()">FULL</button>
            <button id="freeze-btn" class="action-btn" onclick="toggleFreeze()">FREEZE</button>
            <button id="rec-btn" class="action-btn" onclick="toggleRecording()">REC START</button>
        </div>
    </div>

    <div class="graph-container">
        <div class="label">Connection Force (-3.5 kg to +3.5 kg)</div>
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

    function toggleFreeze() {
        isFrozen = !isFrozen;
        if (isFrozen) {
            freezeBtn.innerText = "RUNNING";
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

            let y_kraft = 125 - (currentW / 3500) * 125;
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
            let y_lg = 125 - (leftQuality / 300) * 125;
            y_lg = Math.max(0, Math.min(250, y_lg));
            let isLeftErr = (serverError === 1) || (Math.abs(currentLA) > 1.5 && Math.abs(currentLG) < 80.0);

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
            let y_rg = 125 - (rightQuality / 300) * 125;
            y_rg = Math.max(0, Math.min(250, y_rg));
            let isRightErr = (serverError === 2) || (Math.abs(currentRA) > 1.5 && Math.abs(currentRG) < 80.0);

            rightFootPoints.shift(); rightFootPoints.push({ y: y_rg, isError: isRightErr });
        } else {
            rightFootPoints.shift(); rightFootPoints.push({ y: null, isError: false });
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