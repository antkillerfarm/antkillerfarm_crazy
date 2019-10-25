var c = document.getElementById("myCanvas");
var g = c.getContext("2d");

function init_3d(a, b, c, val) {
    var arr = new Array(a);
    for (let i = 0; i < a; i++) {
        arr[i] = new Array(b);
        for (let j = 0; j < b; j++) {
            arr[i][j] = new Array(c).fill(val);
        }
    }
    return arr;
}

function fillPolygon(x, y, len) {
    g.beginPath();
    g.moveTo(x[0], y[0]);
    for (let i = 1; i < len; i++) {
        g.lineTo(x[i], y[i]);
    }
    g.closePath();
    g.fill();
}

function drawLine(x0, y0, x1, y1) {
    g.beginPath();
    g.moveTo(x0, y0);
    g.lineTo(x1, y1);
    g.stroke();
}

function fillArc(x, y, r) {
    g.beginPath();
    g.arc(x, y, r, 0, Math.PI * 2, true);
    g.closePath();
    g.fill();
}

class Q_Env {
    constructor() {
        this.numberOfSteps = 0;
        this.totalReward = 0.0;
        this.currX = Math.floor(Math.random() * 10);
        this.currY = Math.floor(Math.random() * 10);
        this.tracing = false;
    }
    doreset() {
        this.numberOfSteps = 0;
        this.totalReward = 0.0;
    }
    dostep(action) {
        let actualDirection;
        let reward;
        let newX, newY;
        let rand = Math.floor(Math.random() * 10);
        if (rand < 4) actualDirection = rand;
        else actualDirection = action;

        reward = 0.0;
        if ((this.currX == 8 && this.currY == 7) ||
            (this.currX == 7 && this.currY == 2)) {
            if (this.currX == 8)
                reward = 10.0;
            else
                reward = 3.0;
            // Go to a random corner
            newX = Math.floor(Math.random() * 2) * 9;
            newY = Math.floor(Math.random() * 2) * 9;
        } else {
            if (this.currX == 3 && this.currY == 4)
                reward = -5.0;
            else if (this.currX == 3 && this.currY == 7)
                reward = -10.0;

            // create new position & get hitting-wall rewards
            switch (actualDirection) {
                case 0: // Up
                    if (this.currY == 0) {
                        newY = this.currY;
                        newX = this.currX;
                        reward = -1.0;
                    } else {
                        newY = this.currY - 1;
                        newX = this.currX;
                    }
                    break;
                case 1: // Right
                    if (this.currX == 9) {
                        newY = this.currY;
                        newX = this.currX;
                        reward = -1.0;
                    } else {
                        newY = this.currY;
                        newX = this.currX + 1;
                    }
                    break;
                case 2: // Down
                    if (this.currY == 9) {
                        newY = this.currY;
                        newX = this.currX;
                        reward = -1.0;
                    } else {
                        newY = this.currY + 1;
                        newX = this.currX;
                    }
                    break;
                case 3: // Left
                    if (this.currX == 0) {
                        newY = this.currY;
                        newX = this.currX;
                        reward = -1.0;
                    } else {
                        newY = this.currY;
                        newX = this.currX - 1;
                    }
                    break;
                default: // should never occur
                {
                    newX = 0;
                    newY = 0;
                    reward = 0.0;
                }
            }
        }


        this.numberOfSteps++;
        this.totalReward += reward;

        this.currX = newX;
        this.currY = newY;
        return reward;
    }
}

class Q_Controller {
    constructor(env) {
        this.environment = env;

        this.qvalues = init_3d(10, 10, 4, 0.0);
        this.visits = init_3d(10, 10, 4, 0);
        this.discount = 0.9;
        this.alphaFixed = false;
        this.alpha = 0.0;
        this.tracing = false;
    }
    doreset(initVal) {
        for (let xval = 0; xval < 10; xval++) {
            for (let yval = 0; yval < 10; yval++) {
                for (let i = 0; i < 4; i++) {
                    this.qvalues[xval][yval][i] = initVal;
                    this.visits[xval][yval][i] = 0;
                }
            }
        }
    }
    dostep0(action, newdiscount, alphaFieldValue) {
        this.discount = newdiscount;
        this.alpha = alphaFieldValue;
        dostep(action);
    }
    dostep(action) {
        let oldX = this.environment.currX;
        let oldY = this.environment.currY;
        let reward = this.environment.dostep(action);

        let newX = this.environment.currX;
        let newY = this.environment.currY;

        // update Q values
        let newVal = this.value(newX, newY);
        let newDatum = reward + this.discount * newVal;
        this.visits[oldX][oldY][action]++;
        if (!this.alphaFixed)
            this.alpha = 1.0 / this.visits[oldX][oldY][action];
        //	    this.alpha = 10.0/(9+this.visits[oldX][oldY][action]);

        if (this.tracing) {
            console.log("(" + oldX + "," + oldY + ") A=" + action + " R=" + reward + " (" +
                newX + "," + newY + ") newDatum=" + newDatum);
            console.log("     Qold=" + this.qvalues[oldX][oldY][action] + " this.visits=" + this.visits[oldX][oldY][action]);
        }

        this.qvalues[oldX][oldY][action] =
            (1 - this.alpha) * this.qvalues[oldX][oldY][action] +
            this.alpha * newDatum;
        if (this.tracing) console.log(" Qnew=" + this.qvalues[oldX][oldY][action]);
    }
    value(xval, yval) {
        let val = this.qvalues[xval][yval][3];
        for (let i = 2; i >= 0; i--) {
            if (this.qvalues[xval][yval][i] > val) {
                val = this.qvalues[xval][yval][i];
            }
        }
        return val;
    }
    doSteps(count, greedyProb, newdiscount, alphaFieldValue) {
        this.discount = newdiscount;
        this.alpha = alphaFieldValue;
        for (let i = 0; i < count; i++) {
            let rand = Math.random();
            if (rand < greedyProb) { // act greedily
                let startDir = Math.floor(Math.random() * 4);
                let bestVal = this.qvalues[environment.currX][environment.currY][startDir];
                let bestDir = startDir;
                for (let dir = 1; dir < 4; dir++) {
                    startDir = (startDir + 1) % 4;
                    if (this.qvalues[environment.currX][environment.currY][startDir] > bestVal) {
                        bestVal = this.qvalues[environment.currX][environment.currY][startDir];
                        bestDir = startDir;
                    }
                }
                dostep(bestDir);
            } else { // act randomly
                dostep(Math.floor(Math.random() * 4));
            }
        }
    }
}

class Q_GUI {
    constructor() {
        this.environment = new Q_Env();
        this.controller = new Q_Controller(this.environment);
        this.sqsize = 60;
        this.twid = 5;
        this.brightness = 1.0;
    }

    paintComponent(g) {
        for (let xval = 0; xval < 10; xval++) {
            for (let yval = 0; yval < 10; yval++) {
                let val = this.controller.value(xval, yval);
                let c0;
                if (val >= 0.0) {
                    c0 = Math.min(Math.floor(255.0 *
                        Math.pow(val / 10.0, this.brightness)), 255);
                    g.fillStyle = "RGB(0, " + c0 + ", 0)";
                } else {
                    c0 = Math.min(Math.floor(255.0 *
                        Math.pow(-val / 10.0, this.brightness)), 255);
                    g.fillStyle = "RGB(" + c0 + ", 0, 0)";
                }

                g.fillRect(xval * this.sqsize, yval * this.sqsize,
                    this.sqsize, this.sqsize);
                g.fillStyle = "Blue";
                if (val == this.controller.qvalues[xval][yval][0]) {
                    let uptrix = [xval * this.sqsize + this.sqsize / 2 - this.twid,
                        xval * this.sqsize + this.sqsize / 2 + this.twid,
                        xval * this.sqsize + this.sqsize / 2
                    ];
                    let uptriy = [yval * this.sqsize + this.sqsize / 2,
                        yval * this.sqsize + this.sqsize / 2,
                        yval * this.sqsize
                    ];
                    fillPolygon(uptrix, uptriy, 3);
                }
                if (val == this.controller.qvalues[xval][yval][1]) {
                    let uptriy = [
                        yval * this.sqsize + this.sqsize / 2 - this.twid,
                        yval * this.sqsize + this.sqsize / 2 + this.twid,
                        yval * this.sqsize + this.sqsize / 2
                    ];
                    let uptrix = [
                        xval * this.sqsize + this.sqsize / 2,
                        xval * this.sqsize + this.sqsize / 2,
                        (xval + 1) * this.sqsize
                    ];
                    fillPolygon(uptrix, uptriy, 3);
                }
                if (val == this.controller.qvalues[xval][yval][2]) {
                    let uptrix = [
                        xval * this.sqsize + this.sqsize / 2 - this.twid,
                        xval * this.sqsize + this.sqsize / 2 + this.twid,
                        xval * this.sqsize + this.sqsize / 2
                    ];
                    let uptriy = [
                        yval * this.sqsize + this.sqsize / 2,
                        yval * this.sqsize + this.sqsize / 2,
                        (yval + 1) * this.sqsize
                    ];
                    fillPolygon(uptrix, uptriy, 3);
                }
                if (val == this.controller.qvalues[xval][yval][3]) {
                    let uptriy = [
                        yval * this.sqsize + this.sqsize / 2 - this.twid,
                        yval * this.sqsize + this.sqsize / 2 + this.twid,
                        yval * this.sqsize + this.sqsize / 2
                    ];
                    let uptrix = [
                        xval * this.sqsize + this.sqsize / 2,
                        xval * this.sqsize + this.sqsize / 2,
                        xval * this.sqsize
                    ];
                    fillPolygon(uptrix, uptriy, 3);
                }
                g.fillStyle = "White";
                g.fillText(this.controller.qvalues[xval][yval][0],
                    xval * this.sqsize + this.sqsize / 3,
                    yval * this.sqsize + this.sqsize / 3);
                g.fillText(this.controller.qvalues[xval][yval][1],
                    xval * this.sqsize + 2 + this.sqsize / 2,
                    yval * this.sqsize + 2 * this.sqsize / 3);
                g.fillText(this.controller.qvalues[xval][yval][2],
                    xval * this.sqsize + this.sqsize / 3,
                    (yval + 1) * this.sqsize - 1);
                g.fillText(this.controller.qvalues[xval][yval][3],
                    xval * this.sqsize + 2,
                    yval * this.sqsize + 2 * this.sqsize / 3);
            }
        }
        g.strokeStyle = "RGB(0, 0, 255)";
        drawLine(0, 0, 0, 10 * this.sqsize);
        drawLine(0, 0, 10 * this.sqsize, 0);
        drawLine(10 * this.sqsize, 0, 10 * this.sqsize, 10 * this.sqsize);
        drawLine(0, 10 * this.sqsize, 10 * this.sqsize, 10 * this.sqsize);
        g.strokeStyle = "White";
        for (let counter = 1; counter <= 10; counter++) {
            drawLine(this.sqsize * counter, 0,
                this.sqsize * counter, 10 * this.sqsize);
            drawLine(0, this.sqsize * counter,
                10 * this.sqsize, this.sqsize * counter);
        }
        g.fillStyle = "Yellow";
        fillArc(this.environment.currX * this.sqsize + this.sqsize / 2,
            this.environment.currY * this.sqsize + this.sqsize / 2,
            this.sqsize - 2 * (this.sqsize / 3))
        $("#report_steps_label").html(this.environment.numberOfSteps);
        $("#report_rewards_label").html(this.environment.totalReward);
    }
    dostep(action) {
        this.controller.dostep(action,
            0,
            0.4);
    }
}

var q = new Q_GUI();
q.paintComponent(g);


function onButton(action) {
    console.log(action);
    q.dostep(action);
    q.paintComponent(g);
}

$("#up").click(function () {
    onButton(0);
});

$("#right").click(function () {
    onButton(1);
});

$("#down").click(function () {
    onButton(2);
});

$("#left").click(function () {
    onButton(3);
});