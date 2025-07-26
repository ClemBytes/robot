/**
 * @file robot.js
 * @brief Script for interacting with the robot grid.
 */

/**
 * Handle keydown events to simulate button clicks.
 * 
 * @param {KeyboardEvent} event - The keydown event.
 */
function onKeyDown(event) {
    if (event.key == "ArrowRight") {
        fetch("/right", { method: "POST" }).then(replaceGrid);
        event.preventDefault();
    } else if (event.key == "ArrowLeft") {
        fetch("/left", { method: "POST" }).then(replaceGrid);
        event.preventDefault();
    } else if (event.key == "ArrowUp") {
        fetch("/up", { method: "POST" }).then(replaceGrid);
        event.preventDefault();
    } else if (event.key == "ArrowDown") {
        fetch("/down", { method: "POST" }).then(replaceGrid);
        event.preventDefault();
    } else if (event.key == "Backspace") {
        fetch("/resetn", { method: "POST" }).then(replaceGrid);
        event.preventDefault();
    } else {
        console.log(event);
    }
}
document.addEventListener("keydown", onKeyDown);

/**
 * Handle click events on the grid.
 * Redirects to the corresponding cell coordinates.
 * 
 * @param {MouseEvent} event - The click event.
 */
function onClick(event) {
    if (event.target.localName == "td") {
        const x = event.target.dataset.x;
        const y = event.target.dataset.y;
        document.location = "/coords/" + x + "/" + y;
    }
}
const robot_grid = document.getElementById("robot-grid");
robot_grid.addEventListener("click", onClick);

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
    console.log(x_coord);
    console.log(y_coord);
    for (let x=0; x<x_max; x++) {
        grid += "<tr>";
        for (y=0; y<y_max; y++) {
            if (x == x_coord && y == y_coord) {
                grid += `<td data-x='${x}' data-y='${y}'><img src='/data/robot.png' alt='Robot' class='image-responsive'></td>`;
            } else {
                grid += `<td data-x='${x}' data-y='${y}'></td>`;
            }
        }
        grid += "</tr>";
    }
    console.log(grid);
    document.getElementById("robot-grid").innerHTML = grid;
}

/**
 * @brief Replace grid by newly generated one (with new robot's coordinates).
 */
function replaceGrid() {
    const [x_coord, y_coord] = readCookies();
    generateGrid(x_coord, y_coord);
}