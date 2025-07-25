/**
 * @file robot.js
 * @brief Script for interacting with the robot grid.
 */

/**
 * @brief Handle keydown events for moving robot.
 * 
 * @param {KeyboardEvent} event - The keydown event.
 */
async function onKeyDown(event) {
    try {
        if (event.key == "ArrowRight") {
            event.preventDefault();
            await fetch("/right", { method: "POST" });
        } else if (event.key == "ArrowLeft") {
            event.preventDefault();
            await fetch("/left", { method: "POST" });
        } else if (event.key == "ArrowUp") {
            event.preventDefault();
            await fetch("/up", { method: "POST" });
        } else if (event.key == "ArrowDown") {
            event.preventDefault();
            await fetch("/down", { method: "POST" });
        } else if (event.key == "Backspace") {
            event.preventDefault();
            await fetch("/reset", { method: "POST" });
        } else {
            console.log(event);
            return;
        }
        replaceGrid();
    } catch {
        alert("Could not reach server!");
    }
}
document.addEventListener("keydown", onKeyDown);

/**
 * @brief Handle click events (on grid or buttons).
 * 
 * @param {MouseEvent} event - The click event.
 */
async function onClick(event) {
    event.preventDefault();
    try {
        if (event.target.localName == "td") {
            const x = event.target.dataset.x;
            const y = event.target.dataset.y;
            await fetch(`/coords/${x}/${y}`, { method: "GET" });
        } else if (event.target.id == "up") {
            await fetch("/up", { method: "POST" });
        } else if (event.target.id == "down") {
            await fetch("/down", { method: "POST" });
        } else if (event.target.id == "right") {
            await fetch("/right", { method: "POST" });
        } else if (event.target.id == "left") {
            await fetch("/left", { method: "POST" });
        } else if (event.target.id == "reset") {
            await fetch("/reset", { method: "POST" });
        } else {
            console.log(event);
            return;
        }
        replaceGrid();
    } catch {
        alert("Could not reach server!");
    }
}
const robot_grid = document.getElementById("robot-grid");
robot_grid.addEventListener("click", onClick);

const up_button = document.getElementById("up");
up_button.addEventListener("click", onClick);

const down_button = document.getElementById("down");
down_button.addEventListener("click", onClick);

const right_button = document.getElementById("right");
right_button.addEventListener("click", onClick);

const left_button = document.getElementById("left");
left_button.addEventListener("click", onClick);

const reset_button = document.getElementById("reset");
reset_button.addEventListener("click", onClick);

/**
 * @brief Reads cookies of document.
 */
function readCookies() {
    const cookies_array = document.cookie.split("; ");
    const x = cookies_array.find((row) => row.startsWith("x=")).split("=")[1];
    const y = cookies_array.find((row) => row.startsWith("y=")).split("=")[1];
    return [x, y];
}

/**
 * @brief Generate grid given robot's coordinate and size of grid.
 * 
 * @param {int} x_coord Robot's x coordinate.
 * @param {int} y_coord Robot's y coordinate.
 * @param {int} x_max Value max for x (so number of lines in the grid - 1).
 * @param {int} y_max Value max for y (so number of columns in the grid - 1).
 */
function generateGrid(x_coord, y_coord, x_max=5, y_max=5) {
    let grid = "";
    for (let x = 0; x < x_max; x++) {
        grid += "<tr>";
        for (let y = 0; y < y_max; y++) {
            if (x == x_coord && y == y_coord) {
                grid += `<td data-x='${x}' data-y='${y}'><img src='/data/robot.png' alt='Robot' class='image-responsive'></td>`;
            } else {
                grid += `<td data-x='${x}' data-y='${y}'></td>`;
            }
        }
        grid += "</tr>";
    }
    document.getElementById("robot-grid").innerHTML = grid;
}

/**
 * @brief Updates coordinates written on top of page.
 * 
 * @param {int} x_coord Robot's x coordinate.
 * @param {int} y_coord Robot's y coordinate.
 */
function updateCoords(x_coord, y_coord) {
    document.getElementById("coords").outerHTML = `<p id="coords">Current coordinates: (${x_coord}, ${y_coord})</p>`;
}

/**
 * @brief Replace grid by newly generated one (with new robot's coordinates).
 */
function replaceGrid() {
    const [x_coord, y_coord] = readCookies();
    generateGrid(x_coord, y_coord);
    updateCoords(x_coord, y_coord);
}