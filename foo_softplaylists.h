
// general string manipulations
pfc::string stringcvt_escape_xml(pfc::stringp in);
pfc::string stringcvt_escape_url(pfc::stringp in);

// last.fm feed downloading
pfc::string8 http_get_xml(pfc::string8 url, abort_callback& p_abort);
bool download_user_loved_tracks_feed_pages(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort);
bool download_user_top_tracks_and_playcounts_feed_pages(pfc::string8 lastfm_user, pfc::string8 period, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, pfc::list_t<t_size>& track_playcounts, abort_callback& p_abort);
bool download_user_recent_tracks_feed_pages(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort);
bool download_artist_top_tracks_and_playcounts_feed_pages(pfc::string8 artist_name, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, pfc::list_t<t_size>& track_playcounts, abort_callback& p_abort);
bool download_tag_top_tracks_feed_pages(pfc::string8 lastfm_tag, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort);
bool download_user_friends_feed(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& users, abort_callback& p_abort);
bool download_user_neighbours_feed(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& users, abort_callback& p_abort);
bool download_artist_similar_artists_feed(pfc::string8 artist_name, pfc::list_t<pfc::string8>& artists, abort_callback& p_abort);
bool download_similar_tracks_feed(pfc::string8 track_title, pfc::string8& track_artist, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort);
bool download_top_tags_feed(pfc::string8& tags);
bool download_playlists_feed(pfc::string8& playlist_ids, pfc::string8& playlist_names);
bool download_playlist_tracks_feed(pfc::string8 playlist_id, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort);

// matching
void sort_library_extract_lib_track_info(pfc::list_t<metadb_handle_ptr>& library, pfc::list_t<pfc::string8>& lib_track_titles, pfc::list_t<pfc::string8>& lib_track_artists, pfc::list_t<pfc::string8>& lib_track_paths, pfc::list_t<t_size>& lib_track_lengths, pfc::list_t<double>& lib_track_bitrates, pfc::list_t<pfc::string8>& lib_track_albums, pfc::list_t<pfc::string8>& lib_track_dates, pfc::list_t<t_size>& lib_track_trnums);
metadb_handle_ptr get_preferred_track_by_info(pfc::string8 title, pfc::string8 artist, t_size length, pfc::string8 album, pfc::string8 date, t_size trnum, pfc::list_t<metadb_handle_ptr>& library, pfc::list_t<pfc::string8>& lib_track_titles, pfc::list_t<pfc::string8>& lib_track_artists, pfc::list_t<pfc::string8>& lib_track_paths, pfc::list_t<t_size>& lib_track_lengths, pfc::list_t<double>& lib_track_bitrates, pfc::list_t<pfc::string8>& lib_track_albums, pfc::list_t<pfc::string8>& lib_track_dates, pfc::list_t<t_size>& lib_track_trnums);

// get track info
bool get_artist(metadb_handle_list_cref p_data, pfc::string8& track_artist);
bool get_track_title_artist(metadb_handle_list_cref p_data, pfc::string8& track_title, pfc::string8& track_artist);

// love, unlove, ban or unban a track on last.fm user account
bool api_method_track_title_artist(pfc::string8 api_method_name, pfc::string8 track_title, pfc::string8 track_artist, abort_callback& p_abort);

// add a track to a last.fm playlist
bool api_method_playlist_track_title_artist(pfc::string8 api_method_name, pfc::string8 playlist_id, pfc::string8 track_title, pfc::string8 track_artist, abort_callback& p_abort);

// xspf
void load_xspf_playlist(pfc::string8& xspf_filepath, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, pfc::list_t<pfc::string8>& track_albums, pfc::list_t<t_size>& track_lengths, pfc::list_t<t_size>& track_trnums);
void save_xspf_playlist(pfc::string8 xspf_filepath);
