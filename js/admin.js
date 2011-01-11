Drupal.kalturaAdmin = {
    
    init: function() {
        if ( $('#saas-or-ce-wrapper').length ) {
            if ( $('#saas-or-ce-wrapper input[@name=saas_or_ce]:checked').val() != 'ce' ) {
                $('#edit-kaltura-url-wrapper').hide('slow');
            }
            $('#saas-or-ce-wrapper input[@name=saas_or_ce]').click( function() {
                    if ( $('#saas-or-ce-wrapper input[@name=saas_or_ce]:checked').val() != 'ce' ) {
                        $('#edit-kaltura-url-wrapper').hide('slow');
                    }
                    else {
                        $('#edit-kaltura-url-wrapper').show('slow');                        
                    }
                } );
        }
    }

} ;

Drupal.behaviors.kalturaAdmin = Drupal.kalturaAdmin.init ;