$(document).ready(function(){
    $('.buttons').hide();
    $('.label').on('click', function() {
        var controller = $('.controller');
        var buttons = $('.buttons');
        var siblings = $(this).closest(controller).children();

        if(siblings.hasClass('expanded')) {
            controller.children().removeClass('expanded');
            controller.find(buttons).slideUp();
        } else {
            controller.children().removeClass('expanded');
            controller.find(buttons).slideUp('slow');
            $(this).closest(controller).find(buttons).slideDown('fast');
            siblings.addClass('expanded');
        }
    });
    $('button').on('click', function() {
        if ($(this).hasClass('on')) {
            $(this).removeClass('on');
            $(this).text('Toggle off')
            $(this).addClass('off')
            $(this).text('Toggle on')
        } else {
            $(this).removeClass('off');
            $(this).text('Toggle onn')
            $(this).addClass('on')
            $(this).text('Toggle off')
        }

    });
});

