var time_step = 0
var light1 = {
    x: 165,
    y: 200,
    is_red: true,
    is_update: true,
};
var light2 = {
    x: 585,
    y: 200,
    is_red: false,
    is_update: true,
};

var light1x = [110, 180, 220, 150]; //左侧信号灯处的横坐标
var light1y = [235, 275, 165, 125]; //左侧信号灯处的纵坐标
var light2x = [530, 600, 640, 570]; //右侧信号灯处的横坐标
var light2y = [235, 275, 165, 125]; //右侧信号灯处的纵坐标
var turnx = [25, 150, 180, 570, 600, 725]; //转向点的横坐标
//决定转向的点的横坐标
var pre_turn_x = [25, 25, 150, 110, 220, 180, 570, 530, 640, 600, 725, 725];
//决定转向的点的纵坐标
var pre_turn_y = [165, 235, 125, 235, 165, 275, 125, 235, 165, 275, 165, 235];
//决定转向的点上可供选择的转向方向
var can_turn = [
    [1, 2],
    [0, 4],
    [1, 2],
    [0, 1],
    [2, 3],
    [0, 3],
    [1, 2],
    [0, 1],
    [2, 3],
    [0, 3],
    [2, 4],
    [0, 3]
];
var appearx = [0, 750, 150, 180, 570, 600]; //汽车出现地点的横坐标
var appeary = [235, 165, 0, 400, 0, 400]; //汽车出现地点的纵坐标
//汽车出现时的行驶方向，0为向东，1为向南，2为向西，
//3为向北，4为特殊处理时的保留字
var appeard = [0, 2, 1, 3, 1, 3];
var car_list = new Array()

var c = document.getElementById("myCanvas");
var g = c.getContext("2d");

function draw_ground() {
    g.fillStyle = "#FF8000";
    g.fillRect(0, 0, 125, 140);
    g.fillRect(205, 0, 340, 140);
    g.fillRect(625, 0, 125, 140);
    g.fillRect(0, 260, 125, 140);
    g.fillRect(205, 260, 340, 140);
    g.fillRect(625, 260, 125, 140);
    g.fillStyle = "#00C000";
    g.fillRect(45, 190, 80, 20);
    g.fillRect(205, 190, 340, 20);
    g.fillRect(625, 190, 85, 20);
    g.strokeStyle = "#000000";
    g.strokeRect(0, 0, 125, 140);
    g.strokeRect(205, 0, 340, 140);
    g.strokeRect(625, 0, 125, 140);
    g.strokeRect(0, 260, 125, 140);
    g.strokeRect(205, 260, 340, 140);
    g.strokeRect(625, 260, 125, 140);
    g.strokeRect(45, 190, 80, 20);
    g.strokeRect(205, 190, 340, 20);
    g.strokeRect(625, 190, 85, 20);
}

function fillArc(x, y) {
    g.beginPath();
    g.arc(x, y, 5, 0, Math.PI * 2, true);
    g.closePath();
    g.fill();
}

function draw_light(light) {
    var x = light.x;
    var y = light.y;
    if (!light.is_update) return;
    if (light.is_red) {
        g.fillStyle = "Red";
        fillArc(x - 40, y - 75);
        fillArc(x + 40, y + 75);
        g.fillStyle = "Green";
        fillArc(x - 55, y + 60);
        fillArc(x + 55, y - 60);
    } else {
        g.fillStyle = "Green";
        fillArc(x - 40, y - 75);
        fillArc(x + 40, y + 75);
        g.fillStyle = "Red";
        fillArc(x - 55, y + 60);
        fillArc(x + 55, y - 60);
    }
    light.is_update = false;
}

function toggle_light(light) {
    light.is_red = !light.is_red;
    light.is_update = true;
}

function test_crash(x, y, index) {
    for (var i = 0; i < car_list.length; i++) {
        if (index == i) continue;
        deltax = Math.abs(car_list[i].x - x);
        deltay = Math.abs(car_list[i].y - y);
        if (deltax < 30 &&
            deltay < 30) {
            return true;
        }
    }
    return false;
}

//随机产生汽车出现的地点和方向
function generate_car() {
    var i;
    var d = Math.random();
    if (d < 0.3) i = 0;
    else if (d < 0.6) i = 1;
    else if (d < 0.7) i = 2;
    else if (d < 0.8) i = 3;
    else if (d < 0.9) i = 4;
    else i = 5;
    var x = appearx[i];
    var y = appeary[i];
    direct = appeard[i];
    var obj = {
        x: x,
        y: y,
        x0: x,
        y0: y,
        direct: direct,
        pre_direct: direct, //汽车下一步的行驶方向
        is_wait: false,
    }
    if (test_crash(x, y, null)) {
        return {
            val: false,
            car: obj
        };
    } else {
        return {
            val: true,
            car: obj
        };
    }
}

function draw_car(car) {
    g.fillStyle = "White";
    g.fillRect(car.x0 - 10, car.y0 - 10, 20, 20); //清除上一刻的汽车图像
    g.fillStyle = "Red";
    g.fillRect(car.x - 10, car.y - 10, 20, 20); //画出下一刻的汽车图像
}

function draw_cars() {
    car_list.forEach(draw_car);
}

//判断汽车是否在界内
function is_in(car) {
    if (car.x >= -10 && car.x <= 760 &&
        car.y >= -10 && car.y <= 410) return true;
    else return false;
}
//判断汽车是否到了决定转向的点
//因为汽车右转不受信号灯的影响，故转向点并非决定转向的点
function is_preturn(car) {
    for (var i = 0; i < 12; i++) {
        if (car.x == pre_turn_x[i] && car.y == pre_turn_y[i]) {
            return {
                val: true,
                index: i,
            };
        }
    }
    return {
        val: false,
        index: 0,
    };
}
//判断汽车是否到了左侧的信号灯处
function is_light1(car) {
    for (var i = 0; i < 4; i++) {
        if (car.x == light1x[i] && car.y == light1y[i])
            return true;
    }
    return false;
}
//判断汽车是否到了右侧的信号灯处
function is_light2(car) {
    for (var i = 0; i < 4; i++) {
        if (car.x == light2x[i] && car.y == light2y[i])
            return true;
    }
    return false;
}
//判断汽车是否到了转向点
function is_turn(car) {
    for (var i = 0; i < 6; i++) {
        if (car.x == turnx[i] && (car.y == 165 || car.y == 235))
            return true;
    }
    return false;
}

function is_turn_180(car) {
    var direct0 = 4;
    switch (car.direct) {
        case 0:
            direct0 = 2;
            break;
        case 1:
            direct0 = 3;
            break;
        case 2:
            direct0 = 0;
            break;
        default:
            direct0 = 1;
    }
    return (direct0 == car.pre_direct)
}

function handle_light(car, is_light, light) {
    if (is_light(car) && car.direct == car.pre_direct) {
        if (car.direct == 0 || car.direct == 2) { //汽车为东西向行驶时
            if (!light.is_red) {
                car.is_wait = true;
            } else {
                car.is_wait = false;
            }
        } else { //汽车为南北向行驶时
            if (light.is_red) {
                car.is_wait = true;
            } else {
                car.is_wait = false;
            }
        }
        return car.is_wait;
    } else return false;
}

function update_car(car, index, array) {
    var obj = is_preturn(car);
    if (obj.val && !car.is_wait) {
        if (can_turn[obj.index][1] == 4) {
            car.pre_direct = can_turn[obj.index][0]; //只有1种转向
        } else {
            var i; //有2种转向
            if (Math.random() > 0.5) {
                i = 0;
            } else {
                i = 1;
            }
            car.pre_direct = can_turn[obj.index][i];
        }
        if (is_turn_180(car)) {
            console.log("index:" + i)
        }
    }
    //汽车到左侧的信号灯处，且直行时
    if (handle_light(car, is_light1, light1)) return;
    //汽车到右侧的信号灯处，且直行时
    if (handle_light(car, is_light2, light2)) return;

    var direct = car.direct;
    if (is_turn(car)) {
        car.direct = car.pre_direct;
        if (car.x == 25 && car.y == 235) {
            car.direct = 0;
        }
        if (car.x == 725 && car.y == 165) {
            car.direct = 2;
        }
    }
    //根据汽车的行驶方向，确定其下一刻的坐标
    var x, y;
    switch (car.direct) {
        case 0:
            x = car.x + 5;
            y = car.y;
            break;
        case 1:
            x = car.x;
            y = car.y + 5;
            break;
        case 2:
            x = car.x - 5;
            y = car.y;
            break;
        default:
            x = car.x;
            y = car.y - 5;
    }
    if (test_crash(x, y, index)) {
        car.is_wait = true;
        car.direct = direct;
    } else {
        car.x0 = car.x;
        car.y0 = car.y;
        car.x = x;
        car.y = y;
    }

    if (!is_in(car)) {
        array.splice(index, 1);
    }
}

function clean_wait_flag() {
    for (var i = 0; i < car_list.length; i++) {
        car_list[i].is_wait = false;
    }
}

function update_cars() {
    car_list.forEach(update_car);
}

function draw_frame() {
    draw_light(light1);
    draw_light(light2);
    draw_cars();
}

function generate_car_task() {
    if (car_list.length < 50) {
        var obj = generate_car();
        if (obj.val) {
            car_list.push(obj.car);
        }
    }
}

function update_status() {
    if (time_step % 100 == 0) {
        toggle_light(light1);
        toggle_light(light2);
        clean_wait_flag();
    }
    if (time_step % 3 == 0) {
        generate_car_task()
    }
    update_cars()
}

function update_frame() {
    update_status();
    draw_frame();
    time_step++;
    console.log(car_list.length);
}

draw_ground();
draw_frame();

setInterval(update_frame, 100);