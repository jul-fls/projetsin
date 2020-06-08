window.onload = function() {
    horloge("div_clock");
    window.setInterval(function() {
        var date = new Date();
        if (date.getHours() == 9 && date.getMinutes() == 30 && date.getSeconds() == 01) {
            RefreshDistribs();
        }
    }, 1000);

    window.setInterval(function() {
        var date = new Date();
        if (date.getHours() == 21 && date.getMinutes() == 30 && date.getSeconds() == 01) {
            RefreshDistribs();
        }
    }, 1000);
    getDesiredTemp();
    RefreshDistribs();
};

function getDesiredTemp() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("select_desired_temp").options.selectedIndex = this.responseText;
        }
    };

    xhttp.open("GET", "read_desired_temp", true);
    xhttp.send();
}

function horloge(el) {
    if (typeof el == "string") { el = document.getElementById(el); }

    function actualiser() {
        var date = new Date();
        var str = date.getHours();
        str += ':' + (date.getMinutes() < 10 ? '0' : '') + date.getMinutes();
        str += ':' + (date.getSeconds() < 10 ? '0' : '') + date.getSeconds();
        el.innerHTML = str;
    }
    actualiser();
    setInterval(actualiser, 1000);
}

setInterval(function getTemp() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("actual_temp_placeholder").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "read_actual_temp", true);
    xhttp.send();
}, 1000);

function lightonButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "lighton", true);
    xhttp.send();
}

function lightoffButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "lightoff", true);
    xhttp.send();
}

function heatonButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "heaton", true);
    xhttp.send();
}

function heatoffButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "heatoff", true);
    xhttp.send();
}

function auto_heat_changed(checkbox) {
    if (checkbox.checked) {
        alert('Auto Régulation de la température activée !');
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "autoheaton", true);
        xhttp.send();
    } else {
        alert('Auto Régulation de la température désactivée !');
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "autoheatoff", true);
        xhttp.send();
    }
}

function desired_temp_Button() {
    var valeur = $("#select_desired_temp").val();
    $.post("post_desired_temp", {
        DesiredTempValue: valeur
    });
}

function distribButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "distrib", true);
    xhttp.send();
    document.getElementById("last_distrib_placeholder").innerHTML = "A l'instant";
}

function RefreshDistribs() {
    var time = new Date();
    if (time.getHours() >= 9 && time.getHours() <= 20) {
        document.getElementById("last_distrib_placeholder").innerHTML = time.getDate() + "/" + (time.getMonth() + 1) + "/" + time.getFullYear() + " 09:30";
        document.getElementById("next_distrib_placeholder").innerHTML = time.getDate() + "/" + (time.getMonth() + 1) + "/" + time.getFullYear() + " 21:30";
    };
    if (time.getHours() >= 21 && time.getHours() <= 23) {
        document.getElementById("last_distrib_placeholder").innerHTML = time.getDate() + "/" + (time.getMonth() + 1) + "/" + time.getFullYear() + " 21:30";
        document.getElementById("next_distrib_placeholder").innerHTML = (time.getDate() + 1) + "/" + (time.getMonth() + 1) + "/" + time.getFullYear() + " 09:30";
    };
    if (time.getHours() >= 0 && time.getHours() <= 8) {
        document.getElementById("last_distrib_placeholder").innerHTML = (time.getDate() - 1) + "/" + (time.getMonth() + 1) + "/" + time.getFullYear() + " 21:30";
        document.getElementById("next_distrib_placeholder").innerHTML = time.getDate() + "/" + (time.getMonth() + 1) + "/" + time.getFullYear() + " 09:30";
    };
};
.
