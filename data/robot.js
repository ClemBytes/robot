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
        const button_right = document.getElementById("right");
        button_right.click();
        event.preventDefault();
    } else if (event.key == "ArrowLeft") {
        const button_left = document.getElementById("left");
        button_left.click();
        event.preventDefault();
    } else if (event.key == "ArrowUp") {
        const button_up = document.getElementById("up");
        button_up.click();
        event.preventDefault();
    } else if (event.key == "ArrowDown") {
        const button_down = document.getElementById("down");
        button_down.click();
        event.preventDefault();
    } else if (event.key == "Backspace") {
        const button_reset = document.getElementById("reset");
        button_reset.click();
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