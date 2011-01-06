Drupal.kalturaAdmin = {
    
    init: function() {
        if ( $('#edit-server-url').val() == 'http://www.kaltura.com' && $('input[@name=kaltura_registration_mode]:checked').val() == 1 ) {
            $('#fieldset-kaltura-ce').hide() ;
            $('#fieldset-kaltura-saas').show() ;
        }
        $('#edit-kaltura-registration-mode-0').click( function() {
            $('#fieldset-kaltura-ce').show() ;
            $('#fieldset-kaltura-saas').hide() ;
        } ) ;
        $('#edit-kaltura-registration-mode-1').click( function() {
            $('#fieldset-kaltura-ce').hide() ;
            $('#fieldset-kaltura-saas').show() ;
        } ) ;
    }

} ;

Drupal.behaviors.kalturaAdmin = Drupal.kalturaAdmin.init ;