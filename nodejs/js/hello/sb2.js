function super_button_animation(divs, index) {
    $(divs[index]).css("display", "inherit").addClass('fadeInLeft').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function (e) {
        $(this).removeClass('fadeInLeft');
        if (index < divs.length - 1) {
            super_button_animation(divs, index + 1);
        }
    });
}

function plus_btn_clicked() {
    $('#plus_btn').click(function () {
        var div = $('.super_button');
        super_button_animation(div, 0);
        $(this).addClass('rotate_mill').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function (e) {
            $(this).removeClass('rotate_mill');
        });
    });
}

$(document).ready(function () {
    plus_btn_clicked();
});