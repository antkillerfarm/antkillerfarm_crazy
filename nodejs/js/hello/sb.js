function plus_btn_clicked() {
    $('#plus_btn').click(function(){
    $('.super_button').text('JS');
    $(this).addClass('rotate_mill').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function(e) {
            $(this).removeClass('rotate_mill');
        });
    $('.super_button').addClass('fadeInLeft').on('animationend webkitAnimationEnd MSAnimationEnd oAnimationEnd', function(e) {
            $(this).removeClass('fadeInLeft');
        });
    });
}

$(document).ready(function(){
    plus_btn_clicked();
});

