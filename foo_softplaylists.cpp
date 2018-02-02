#include "..\SDK-2010-10-02\foobar2000\ATLHelpers\ATLHelpers.h"

#include <wininet.h>

#include "resource.h"

#include "xmlParser.h"

#include "foo_softplaylists.h"




// plugin name, version, author definitions

#define PLUGINNAME "Soft Playlists"

DECLARE_COMPONENT_VERSION(
	PLUGINNAME, "2011-02-05.2", "(C) 2009-2010 Johan De Bock"
);

VALIDATE_COMPONENT_FILENAME("foo_softplaylists.dll");




// global variables

static pfc::list_t<pfc::string8> spl_global_top_tags;
static pfc::list_t<pfc::string8> spl_global_playlist_ids;
static pfc::list_t<pfc::string8> spl_global_playlist_names;
static pfc::list_t<pfc::string8> spl_global_custom_tags;
static pfc::list_t<pfc::string8> spl_global_custom_users;




// config dialog guids

static const GUID guid_spl_lastfm_user = { 0x2af28116, 0xbc8d, 0x4d16, { 0xb8, 0x7e, 0xe3, 0x5e, 0x2b, 0x4b, 0x14, 0x68 } };
static const GUID guid_spl_lastfm_password_md5 = { 0x3a9e2774, 0x9c71, 0x4dde, { 0x84, 0x1d, 0x21, 0x15, 0x32, 0xd1, 0xb1, 0x3e } };
static const GUID guid_spl_lastfm_password_size = { 0xe967267c, 0x7c2e, 0x4e36, { 0xa0, 0xd7, 0x1c, 0x4f, 0xed, 0x64, 0xfe, 0x1e } };
static const GUID guid_spl_tracks_feed_limit = { 0xa8dae287, 0x113e, 0x4aa8, { 0x9d, 0xdb, 0x58, 0xef, 0x64, 0xb3, 0xc1, 0x65 } };
static const GUID guid_spl_friends_neighbours_feed_limit = { 0xc0b5181c, 0x4334, 0x4360, { 0x91, 0x77, 0xf6, 0x86, 0xd8, 0xc7, 0xfe, 0x50 } };
static const GUID guid_spl_custom_users = { 0x5a54c8c8, 0xe8d8, 0x41cb, { 0xac, 0xa7, 0x29, 0xa5, 0x34, 0x5, 0x94, 0x70 } };
static const GUID guid_spl_similar_artists_feed_limit = { 0xaf0a26e1, 0xac3c, 0x4371, { 0x8c, 0xbb, 0x3a, 0x8a, 0x64, 0x27, 0x32, 0xd8 } };
static const GUID guid_spl_similar_tracks_feed_limit = { 0xdf954488, 0xb878, 0x4c51, { 0xb1, 0x59, 0x57, 0x1f, 0xe0, 0xff, 0xde, 0xa3 } };
static const GUID guid_spl_similar_tracks_recursion_limit = { 0x5aceeba, 0xb515, 0x4d03, { 0x94, 0x35, 0x3c, 0x7c, 0xd7, 0x86, 0x6d, 0x49 } };
static const GUID guid_spl_top_tags_feed_limit = { 0xa1f84aa4, 0x6aea, 0x4264, { 0xbd, 0xe8, 0xe2, 0x1f, 0x84, 0xd7, 0x69, 0xba } };
static const GUID guid_spl_top_tags = { 0x4e0e9ca, 0x6989, 0x43b4, { 0x83, 0x5d, 0xc1, 0x62, 0x40, 0x9a, 0xa5, 0xf2 } };
static const GUID guid_spl_playlist_ids = { 0x2ba47102, 0xf571, 0x44e0, { 0x89, 0x6a, 0xbb, 0x9, 0x38, 0xd3, 0xbb, 0x7f } };
static const GUID guid_spl_playlist_names = { 0x260aaede, 0x838f, 0x4475, { 0x93, 0x3c, 0xd, 0x95, 0xcf, 0xf7, 0xeb, 0x28 } };
static const GUID guid_spl_custom_tags = { 0xde672d37, 0xf94a, 0x4d98, { 0xbc, 0x81, 0x54, 0x1d, 0x14, 0xa, 0x5a, 0x2c } };
static const GUID guid_spl_path_preference = { 0x4b693564, 0x9774, 0x46f2, { 0xa2, 0xf6, 0x91, 0x74, 0xe8, 0x8a, 0x7a, 0xc6 } };
static const GUID guid_spl_unmatched_to_console = { 0xd6e8090e, 0x9c94, 0x41b7, { 0xaf, 0x29, 0xfb, 0x92, 0xbe, 0xbb, 0x0, 0x68 } };
static const GUID guid_spl_prefix_playlists = { 0x5e870b95, 0x486a, 0x4e73, { 0x98, 0x9b, 0xd0, 0xcc, 0x86, 0x6a, 0xed, 0xe1 } };
static const GUID guid_spl_partial_title = { 0xab3f614d, 0x143a, 0x4611, { 0xa3, 0x3f, 0x6c, 0x70, 0xc5, 0xfb, 0xb5, 0x16 } };
static const GUID guid_spl_track_preference_selection = { 0xb74977e, 0x913c, 0x4bcd, { 0xae, 0xaf, 0xc7, 0x35, 0x2c, 0x87, 0x65, 0x9c } };




// config dialog settings

static cfg_string spl_lastfm_user(guid_spl_lastfm_user, "your_user_name");
static cfg_string spl_lastfm_password_md5(guid_spl_lastfm_password_md5, "");
static cfg_uint spl_lastfm_password_size(guid_spl_lastfm_password_size, 0);
static cfg_uint spl_tracks_feed_limit(guid_spl_tracks_feed_limit, 500);
static cfg_uint spl_friends_neighbours_feed_limit(guid_spl_friends_neighbours_feed_limit, 10);
static cfg_string spl_custom_users(guid_spl_custom_users, "");
static cfg_uint spl_similar_artists_feed_limit(guid_spl_similar_artists_feed_limit, 10);
static cfg_uint spl_similar_tracks_feed_limit(guid_spl_similar_tracks_feed_limit, 20);
static cfg_uint spl_similar_tracks_recursion_limit(guid_spl_similar_tracks_recursion_limit, 0);
static cfg_uint spl_top_tags_feed_limit(guid_spl_top_tags_feed_limit, 20);
static cfg_string spl_top_tags(guid_spl_top_tags, "");
static cfg_string spl_playlist_ids(guid_spl_playlist_ids, "");
static cfg_string spl_playlist_names(guid_spl_playlist_names, "");
static cfg_string spl_custom_tags(guid_spl_custom_tags, "60s|70s|80s|90s|acoustic|alternative|ambient|asian|avant-garde|bluegrass|blues|breaks|classic rock|classical|country|dance|disco|dream pop|drum and bass|dubstep|electronic|emo|experimental|folk|funk|garage|goth|grime|grindcore|grunge|hardcore|hip-hop|house|idm|indie|industrial|j-pop|jazz|kids|latin|lounge|metal|musical|new age|new wave|no wave|noise|noise rock|ost|pop|pop punk|post-punk|post-rock|progressive rock|proto-punk|psychedelic|psytrance|punk|rap|reggae|rnb|rock|screamo|ska|soul|synthpop|techno|trance|trip-hop|uk garage|video game music|world|xmas");
static cfg_string spl_path_preference(guid_spl_path_preference, "");
static cfg_bool spl_unmatched_to_console(guid_spl_unmatched_to_console, false);
static cfg_bool spl_prefix_playlists(guid_spl_prefix_playlists, false);
static cfg_bool spl_partial_title(guid_spl_partial_title, false);
static cfg_uint spl_track_preference_selection(guid_spl_track_preference_selection, 1);




// preferences_page

class preferences_page_instance_spl : public CDialogImpl<preferences_page_instance_spl>, public preferences_page_instance {
public:
	preferences_page_instance_spl(preferences_page_callback::ptr callback) : m_callback(callback) {}

	enum {IDD = IDD_DIALOG1};
	t_uint32 get_state();
	void apply();
	void reset();

	BEGIN_MSG_MAP(preferences_page_instance_spl)
	MSG_WM_INITDIALOG(OnInitDialog)
	COMMAND_HANDLER_EX(IDC_EDIT1, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT2, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT3, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT4, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT5, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT6, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT7, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT8, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT9, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT10, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_EDIT11, EN_CHANGE, OnEditChange)
	COMMAND_HANDLER_EX(IDC_CHECK1, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_CHECK2, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_CHECK3, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_RADIO1, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_RADIO2, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_RADIO3, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_RADIO4, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_RADIO5, BN_CLICKED, OnEditChange)
	COMMAND_HANDLER_EX(IDC_RADIO6, BN_CLICKED, OnEditChange)
	END_MSG_MAP()
private:
	BOOL OnInitDialog(CWindow, LPARAM);
	void OnEditChange(UINT, int, CWindow);
	bool HasChanged();
	void OnChanged();

	const preferences_page_callback::ptr m_callback;
};

BOOL preferences_page_instance_spl::OnInitDialog(CWindow, LPARAM)
{
	uSetDlgItemText(m_hWnd, IDC_EDIT1, spl_lastfm_user);
	pfc::string8 fakestring;

	for (t_size i = 0 ; i < spl_lastfm_password_size; i++) {
		fakestring << "f";
	}

	uSetDlgItemText(m_hWnd, IDC_EDIT2, fakestring);
	SetDlgItemInt(IDC_EDIT3, spl_tracks_feed_limit, 0);
	SetDlgItemInt(IDC_EDIT4, spl_friends_neighbours_feed_limit, 0);
	uSetDlgItemText(m_hWnd, IDC_EDIT5, spl_custom_users);
	SetDlgItemInt(IDC_EDIT6, spl_similar_artists_feed_limit, 0);
	SetDlgItemInt(IDC_EDIT7, spl_similar_tracks_feed_limit, 0);
	SetDlgItemInt(IDC_EDIT8, spl_similar_tracks_recursion_limit, 0);
	SetDlgItemInt(IDC_EDIT9, spl_top_tags_feed_limit, 0);
	uSetDlgItemText(m_hWnd, IDC_EDIT10, spl_custom_tags);
	uSetDlgItemText(m_hWnd, IDC_EDIT11, spl_path_preference);
	uButton_SetCheck(m_hWnd, IDC_CHECK1, spl_unmatched_to_console);
	uButton_SetCheck(m_hWnd, IDC_CHECK2, spl_prefix_playlists);
	uButton_SetCheck(m_hWnd, IDC_CHECK3, spl_partial_title);

	if (spl_track_preference_selection == 1) {
		uButton_SetCheck(m_hWnd, IDC_RADIO1, true);
		uButton_SetCheck(m_hWnd, IDC_RADIO2, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO3, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO4, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO5, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO6, false);
	}

	if (spl_track_preference_selection == 2) {
		uButton_SetCheck(m_hWnd, IDC_RADIO1, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO2, true);
		uButton_SetCheck(m_hWnd, IDC_RADIO3, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO4, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO5, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO6, false);
	}

	if (spl_track_preference_selection == 3) {
		uButton_SetCheck(m_hWnd, IDC_RADIO1, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO2, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO3, true);
		uButton_SetCheck(m_hWnd, IDC_RADIO4, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO5, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO6, false);
	}

	if (spl_track_preference_selection == 4) {
		uButton_SetCheck(m_hWnd, IDC_RADIO1, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO2, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO3, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO4, true);
		uButton_SetCheck(m_hWnd, IDC_RADIO5, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO6, false);
	}

	if (spl_track_preference_selection == 5) {
		uButton_SetCheck(m_hWnd, IDC_RADIO1, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO2, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO3, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO4, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO5, true);
		uButton_SetCheck(m_hWnd, IDC_RADIO6, false);
	}

	if (spl_track_preference_selection == 6) {
		uButton_SetCheck(m_hWnd, IDC_RADIO1, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO2, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO3, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO4, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO5, false);
		uButton_SetCheck(m_hWnd, IDC_RADIO6, true);
	}

	return(FALSE);
}

void preferences_page_instance_spl::OnEditChange(UINT, int, CWindow)
{
	OnChanged();
}

t_uint32 preferences_page_instance_spl::get_state()
{
	t_uint32 state = preferences_state::resettable;

	if (HasChanged()) {
		state |= preferences_state::changed;
	}

	return(state);
}

void preferences_page_instance_spl::reset()
{
	uSetDlgItemText(m_hWnd, IDC_EDIT1, "your_user_name");
	uSetDlgItemText(m_hWnd, IDC_EDIT2, "");
	spl_lastfm_password_size = 0;
	SetDlgItemInt(IDC_EDIT3, 500, 0);
	SetDlgItemInt(IDC_EDIT4, 10, 0);
	uSetDlgItemText(m_hWnd, IDC_EDIT5, "");
	SetDlgItemInt(IDC_EDIT6, 10, 0);
	SetDlgItemInt(IDC_EDIT7, 20, 0);
	SetDlgItemInt(IDC_EDIT8, 0, 0);
	SetDlgItemInt(IDC_EDIT9, 20, 0);
	uSetDlgItemText(m_hWnd, IDC_EDIT10, "60s|70s|80s|90s|acoustic|alternative|ambient|asian|avant-garde|bluegrass|blues|breaks|classic rock|classical|country|dance|disco|dream pop|drum and bass|dubstep|electronic|emo|experimental|folk|funk|garage|goth|grime|grindcore|grunge|hardcore|hip-hop|house|idm|indie|industrial|j-pop|jazz|kids|latin|lounge|metal|musical|new age|new wave|no wave|noise|noise rock|ost|pop|pop punk|post-punk|post-rock|progressive rock|proto-punk|psychedelic|psytrance|punk|rap|reggae|rnb|rock|screamo|ska|soul|synthpop|techno|trance|trip-hop|uk garage|video game music|world|xmas");
	uSetDlgItemText(m_hWnd, IDC_EDIT11, "");
	uButton_SetCheck(m_hWnd, IDC_CHECK1, false);
	uButton_SetCheck(m_hWnd, IDC_CHECK2, false);
	uButton_SetCheck(m_hWnd, IDC_CHECK3, false);
	uButton_SetCheck(m_hWnd, IDC_RADIO1, true);
	uButton_SetCheck(m_hWnd, IDC_RADIO2, false);
	uButton_SetCheck(m_hWnd, IDC_RADIO3, false);
	uButton_SetCheck(m_hWnd, IDC_RADIO4, false);
	uButton_SetCheck(m_hWnd, IDC_RADIO5, false);
	uButton_SetCheck(m_hWnd, IDC_RADIO6, false);
	OnChanged();
}

void preferences_page_instance_spl::apply()
{
	uGetDlgItemText(m_hWnd, IDC_EDIT1, spl_lastfm_user);
	pfc::string8 teststring;
	uGetDlgItemText(m_hWnd, IDC_EDIT2, teststring);
	bool match = 1;

	if (strlen(teststring) == spl_lastfm_password_size) {
		for (t_size i = 0 ; i < spl_lastfm_password_size; i++) {
			if (teststring[i] != 'f') {
				match = 0;
				break;
			}
		}
	} else {
		match = 0;
	}

	if (!match) {
		static_api_ptr_t<hasher_md5> hasher;
		spl_lastfm_password_size = (t_uint32)strlen(teststring);
		hasher_md5_result hash = hasher->process_single(teststring, spl_lastfm_password_size);
		spl_lastfm_password_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	}

	spl_tracks_feed_limit = GetDlgItemInt(IDC_EDIT3, 0, 0);

	if (spl_tracks_feed_limit < 10) {
		spl_tracks_feed_limit = 10;
	}

	if (spl_tracks_feed_limit > 10000) {
		spl_tracks_feed_limit = 10000;
	}

	uSetDlgItemInt(IDC_EDIT3, spl_tracks_feed_limit, 0);
	spl_friends_neighbours_feed_limit = GetDlgItemInt(IDC_EDIT4, 0, 0);

	if (spl_friends_neighbours_feed_limit < 1) {
		spl_friends_neighbours_feed_limit = 1;
	}

	if (spl_friends_neighbours_feed_limit > 100) {
		spl_friends_neighbours_feed_limit = 100;
	}

	uSetDlgItemInt(IDC_EDIT4, spl_friends_neighbours_feed_limit, 0);
	uGetDlgItemText(m_hWnd, IDC_EDIT5, spl_custom_users);
	spl_global_custom_users.remove_all();
	splitStringSimple_toList(spl_global_custom_users, "|", spl_custom_users);
	spl_similar_artists_feed_limit = GetDlgItemInt(IDC_EDIT6, 0, 0);

	if (spl_similar_artists_feed_limit < 1) {
		spl_similar_artists_feed_limit = 1;
	}

	if (spl_similar_artists_feed_limit > 100) {
		spl_similar_artists_feed_limit = 100;
	}

	uSetDlgItemInt(IDC_EDIT6, spl_similar_artists_feed_limit, 0);
	spl_similar_tracks_feed_limit = GetDlgItemInt(IDC_EDIT7, 0, 0);

	if (spl_similar_tracks_feed_limit < 10) {
		spl_similar_tracks_feed_limit = 10;
	}

	if (spl_similar_tracks_feed_limit > 10000) {
		spl_similar_tracks_feed_limit = 10000;
	}

	uSetDlgItemInt(IDC_EDIT7, spl_similar_tracks_feed_limit, 0);
	spl_similar_tracks_recursion_limit = GetDlgItemInt(IDC_EDIT8, 0, 0);

	if (spl_similar_tracks_recursion_limit < 0) {
		spl_similar_tracks_recursion_limit = 0;
	}

	if (spl_similar_tracks_recursion_limit > 0) {
		while (pow((double)(int)spl_similar_tracks_feed_limit, (int)spl_similar_tracks_recursion_limit + 1) > 10000) {
			spl_similar_tracks_recursion_limit = spl_similar_tracks_recursion_limit - 1;
		}
	}

	uSetDlgItemInt(IDC_EDIT8, spl_similar_tracks_recursion_limit, 0);
	spl_top_tags_feed_limit = GetDlgItemInt(IDC_EDIT9, 0, 0);

	if (spl_top_tags_feed_limit < 1) {
		spl_top_tags_feed_limit = 1;
	}

	if (spl_top_tags_feed_limit > 100) {
		spl_top_tags_feed_limit = 100;
	}

	if (download_top_tags_feed(spl_top_tags)) {
		spl_global_top_tags.remove_all();
		splitStringSimple_toList(spl_global_top_tags, "|", spl_top_tags);
	}

	uSetDlgItemInt(IDC_EDIT9, spl_top_tags_feed_limit, 0);

	if (download_playlists_feed(spl_playlist_ids, spl_playlist_names)) {
		spl_global_playlist_ids.remove_all();
		splitStringSimple_toList(spl_global_playlist_ids, "|", spl_playlist_ids);
		spl_global_playlist_names.remove_all();
		splitStringSimple_toList(spl_global_playlist_names, "|", spl_playlist_names);
	}

	uGetDlgItemText(m_hWnd, IDC_EDIT10, spl_custom_tags);
	spl_global_custom_tags.remove_all();
	splitStringSimple_toList(spl_global_custom_tags, "|", spl_custom_tags);
	uGetDlgItemText(m_hWnd, IDC_EDIT11, spl_path_preference);
	spl_unmatched_to_console = uButton_GetCheck(m_hWnd, IDC_CHECK1);
	spl_prefix_playlists = uButton_GetCheck(m_hWnd, IDC_CHECK2);
	spl_partial_title = uButton_GetCheck(m_hWnd, IDC_CHECK3);

	if (uButton_GetCheck(m_hWnd, IDC_RADIO1)) {
		spl_track_preference_selection = 1;
	}

	if (uButton_GetCheck(m_hWnd, IDC_RADIO2)) {
		spl_track_preference_selection = 2;
	}

	if (uButton_GetCheck(m_hWnd, IDC_RADIO3)) {
		spl_track_preference_selection = 3;
	}

	if (uButton_GetCheck(m_hWnd, IDC_RADIO4)) {
		spl_track_preference_selection = 4;
	}

	if (uButton_GetCheck(m_hWnd, IDC_RADIO5)) {
		spl_track_preference_selection = 5;
	}

	if (uButton_GetCheck(m_hWnd, IDC_RADIO6)) {
		spl_track_preference_selection = 6;
	}

	OnChanged();
}

bool preferences_page_instance_spl::HasChanged()
{
	bool has_changed = 0;

	if (!has_changed) {
		pfc::string8 teststring;
		uGetDlgItemText(m_hWnd, IDC_EDIT1, teststring);

		if (strcmp(teststring, spl_lastfm_user) != 0) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		pfc::string8 teststring;
		uGetDlgItemText(m_hWnd, IDC_EDIT2, teststring);

		if (strlen(teststring) == spl_lastfm_password_size) {
			for (t_size i = 0 ; i < spl_lastfm_password_size; i++) {
				if (teststring[i] != 'f') {
					has_changed = 1;
					break;
				}
			}
		} else {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_tracks_feed_limit != GetDlgItemInt(IDC_EDIT3, 0, 0)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_friends_neighbours_feed_limit != GetDlgItemInt(IDC_EDIT4, 0, 0)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		pfc::string8 teststring;
		uGetDlgItemText(m_hWnd, IDC_EDIT5, teststring);

		if (strcmp(teststring, spl_custom_users) != 0) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_similar_artists_feed_limit != GetDlgItemInt(IDC_EDIT6, 0, 0)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_similar_tracks_feed_limit != GetDlgItemInt(IDC_EDIT7, 0, 0)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_similar_tracks_recursion_limit != GetDlgItemInt(IDC_EDIT8, 0, 0)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_top_tags_feed_limit != GetDlgItemInt(IDC_EDIT9, 0, 0)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		pfc::string8 teststring;
		uGetDlgItemText(m_hWnd, IDC_EDIT10, teststring);

		if (strcmp(teststring, spl_custom_tags) != 0) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		pfc::string8 teststring;
		uGetDlgItemText(m_hWnd, IDC_EDIT11, teststring);

		if (strcmp(teststring, spl_path_preference) != 0) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_unmatched_to_console != uButton_GetCheck(m_hWnd, IDC_CHECK1)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_prefix_playlists != uButton_GetCheck(m_hWnd, IDC_CHECK2)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (spl_partial_title != uButton_GetCheck(m_hWnd, IDC_CHECK3)) {
			has_changed = 1;
		}
	}

	if (!has_changed) {
		if (uButton_GetCheck(m_hWnd, IDC_RADIO1) && spl_track_preference_selection != 1) {
			has_changed = 1;
		}

		if (uButton_GetCheck(m_hWnd, IDC_RADIO2) && spl_track_preference_selection != 2) {
			has_changed = 1;
		}

		if (uButton_GetCheck(m_hWnd, IDC_RADIO3) && spl_track_preference_selection != 3) {
			has_changed = 1;
		}

		if (uButton_GetCheck(m_hWnd, IDC_RADIO4) && spl_track_preference_selection != 4) {
			has_changed = 1;
		}

		if (uButton_GetCheck(m_hWnd, IDC_RADIO5) && spl_track_preference_selection != 5) {
			has_changed = 1;
		}

		if (uButton_GetCheck(m_hWnd, IDC_RADIO6) && spl_track_preference_selection != 6) {
			has_changed = 1;
		}
	}

	return(has_changed);
}

void preferences_page_instance_spl::OnChanged()
{
	m_callback->on_state_changed();
}

class preferences_page_spl : public preferences_page_impl<preferences_page_instance_spl> {
public:
	const char* get_name() {
		return(PLUGINNAME);
	}
	GUID get_guid() {
		static const GUID guid_preferences_page_spl = { 0x95335236, 0xe531, 0x401f, { 0xb2, 0x3b, 0x80, 0xe8, 0xf, 0x45, 0x5b, 0xb } };
		return(guid_preferences_page_spl);
	}
	GUID get_parent_guid() {
		return(preferences_page::guid_tools);
	}
};
static preferences_page_factory_t<preferences_page_spl> foo_preferences_page_spl;




// initquit

class initquit_spl : public initquit {
	void on_init() {
		spl_global_top_tags.remove_all();
		splitStringSimple_toList(spl_global_top_tags, "|", spl_top_tags);
		spl_global_playlist_ids.remove_all();
		splitStringSimple_toList(spl_global_playlist_ids, "|", spl_playlist_ids);
		spl_global_playlist_names.remove_all();
		splitStringSimple_toList(spl_global_playlist_names, "|", spl_playlist_names);
		spl_global_custom_tags.remove_all();
		splitStringSimple_toList(spl_global_custom_tags, "|", spl_custom_tags);
		spl_global_custom_users.remove_all();
		splitStringSimple_toList(spl_global_custom_users, "|", spl_custom_users);
	}
	void on_quit() {
	}
};
static initquit_factory_t<initquit_spl> foo_initquit_spl;




// general string manipulations

pfc::string stringcvt_escape_xml(pfc::stringp in)
{
	pfc::string out1 = in;
	pfc::string out2;
	out2 = out1.replace("&", "&amp;");
	out1 = out2.replace("'", "&apos;");
	out2 = out1.replace(">", "&gt;");
	out1 = out2.replace("<", "&lt;");
	out2 = out1.replace("\"", "&quot;");
	return(out2);
}

pfc::string stringcvt_escape_url(pfc::stringp in)
{
	pfc::string out1 = in;
	pfc::string out2;
	out2 = out1.replace(" ", "%20");
	out1 = out2.replace("#", "%23");
	out2 = out1.replace("&", "%26");
	out1 = out2.replace("+", "%2B");
	return(out1);
}




// last.fm feed downloading

pfc::string8 http_get_xml(pfc::string8 url, abort_callback& p_abort)
{
	pfc::string8 xml;
	http_client::ptr client;

	try {
		if (!service_enum_t<http_client>().first(client)) {
			console::printf("foo_softplaylists: feed downloading: error: unable to start http service");
			return(xml);
		}

		http_request::ptr request = client->create_request("GET");
		file::ptr file_ptr = request->run(url.get_ptr(), p_abort);
		char buffer[1025];
		t_size bytes_read;

		while (bytes_read = file_ptr->read(buffer, 1024, p_abort)) {
			xml.add_string(buffer, bytes_read);
		}

		return(xml);
	} catch (exception_aborted) {
		console::printf("foo_softplaylists: feed downloading: error: aborted");
		throw exception_aborted();
	} catch (...) {
		console::printf("foo_softplaylists: feed downloading: error");
		return(xml);
	}
}

bool download_user_loved_tracks_feed_pages(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort)
{
	t_size track_iter = 0;
	t_size page_limit = 200;
	t_size number_of_pages = 0;
	{
		t_size page = 1;
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=user.getlovedtracks&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");

		if (root.isEmpty()) {
			console::printf("foo_softplaylists: failed to download Last.fm Loved Tracks feed for user '%s': possibly wrong username", lastfm_user.get_ptr());
			return(0);
		}

		XMLNode xml_list = root.getChildNode("lovedtracks");
		number_of_pages = atoi(xml_list.getAttribute("totalPages"));
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}
			}
		}
	}

	for (t_size page = 2; page <= number_of_pages && track_iter < spl_tracks_feed_limit; page++) {
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=user.getlovedtracks&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");
		XMLNode xml_list = root.getChildNode("lovedtracks");
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}
			}
		}
	}

	return(1);
}

bool download_user_top_tracks_and_playcounts_feed_pages(pfc::string8 lastfm_user, pfc::string8 period, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, pfc::list_t<t_size>& track_playcounts, abort_callback& p_abort)
{
	t_size track_iter = 0;
	t_size page_limit = 200;
	t_size number_of_pages = 0;
	{
		t_size page = 1;
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=user.gettoptracks&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&period=" << period << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");

		if (root.isEmpty()) {
			console::printf("foo_softplaylists: failed to download Last.fm Top Tracks feed for user '%s': possibly wrong username", lastfm_user.get_ptr());
			return(0);
		}

		XMLNode xml_list = root.getChildNode("toptracks");
		number_of_pages = atoi(xml_list.getAttribute("totalPages"));
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}

				if (track.nChildNode("playcount") == 1) {
					track_playcounts.add_item(atoi(track.getChildNode("playcount").getText()));
				} else {
					track_playcounts.add_item(1);
				}
			}
		}
	}

	for (t_size page = 2; page <= number_of_pages && track_iter < spl_tracks_feed_limit; page++) {
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=user.gettoptracks&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&period=" << period << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");
		XMLNode xml_list = root.getChildNode("toptracks");
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}

				if (track.nChildNode("playcount") == 1) {
					track_playcounts.add_item(atoi(track.getChildNode("playcount").getText()));
				} else {
					track_playcounts.add_item(1);
				}
			}
		}
	}

	return(1);
}

bool download_user_recent_tracks_feed_pages(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort)
{
	t_size track_iter = 0;
	t_size page_limit = 200;
	t_size number_of_pages = 0;
	{
		t_size page = 1;
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");

		if (root.isEmpty()) {
			console::printf("foo_softplaylists: failed to download Last.fm Recent Tracks feed for user '%s': possibly wrong username", lastfm_user.get_ptr());
			return(0);
		}

		XMLNode xml_list = root.getChildNode("recenttracks");
		number_of_pages = atoi(xml_list.getAttribute("totalPages"));
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				track_titles.add_item(track.getChildNode("name").getText());
				track_artists.add_item(track.getChildNode("artist").getText());
			}
		}
	}

	for (t_size page = 2; page <= number_of_pages && track_iter < spl_tracks_feed_limit; page++) {
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");
		XMLNode xml_list = root.getChildNode("recenttracks");
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				track_titles.add_item(track.getChildNode("name").getText());
				track_artists.add_item(track.getChildNode("artist").getText());
			}
		}
	}

	return(1);
}

bool download_artist_top_tracks_and_playcounts_feed_pages(pfc::string8 artist_name, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, pfc::list_t<t_size>& track_playcounts, abort_callback& p_abort)
{
	t_size track_iter = 0;
	t_size page_limit = 200;
	t_size number_of_pages = 0;
	{
		t_size page = 1;
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=artist.gettoptracks&artist=" << stringcvt_escape_url(artist_name).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");

		if (root.isEmpty()) {
			console::printf("foo_softplaylists: failed to download Last.fm Top Tracks feed for artist '%s': possibly unknown artist name", artist_name.get_ptr());
			return(0);
		}

		XMLNode xml_list = root.getChildNode("toptracks");
		number_of_pages = atoi(xml_list.getAttribute("totalPages"));
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}

				if (track.nChildNode("playcount") == 1) {
					track_playcounts.add_item(atoi(track.getChildNode("playcount").getText()));
				} else {
					track_playcounts.add_item(1);
				}
			}
		}
	}

	for (t_size page = 2; page <= number_of_pages && track_iter < spl_tracks_feed_limit; page++) {
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=artist.gettoptracks&artist=" << stringcvt_escape_url(artist_name).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");
		XMLNode xml_list = root.getChildNode("toptracks");
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}

				if (track.nChildNode("playcount") == 1) {
					track_playcounts.add_item(atoi(track.getChildNode("playcount").getText()));
				} else {
					track_playcounts.add_item(1);
				}
			}
		}
	}

	return(1);
}

bool download_tag_top_tracks_feed_pages(pfc::string8 lastfm_tag, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort)
{
	t_size track_iter = 0;
	t_size page_limit = 200;
	t_size number_of_pages = 0;
	{
		t_size page = 1;
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=tag.gettoptracks&tag=" << stringcvt_escape_url(lastfm_tag).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");

		if (root.isEmpty()) {
			console::printf("foo_softplaylists: failed to download Last.fm Top Tracks feed for tag '%s': possibly unknown tag", lastfm_tag.get_ptr());
			return(0);
		}

		XMLNode xml_list = root.getChildNode("toptracks");
		number_of_pages = atoi(xml_list.getAttribute("totalPages"));
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}
			}
		}
	}

	for (t_size page = 2; page <= number_of_pages && track_iter < spl_tracks_feed_limit; page++) {
		pfc::string8 url;
		url << "http://ws.audioscrobbler.com/2.0/?method=tag.gettoptracks&tag=" << stringcvt_escape_url(lastfm_tag).get_ptr() << "&limit=" << page_limit << "&page=" << page << "&api_key=b25b959554ed76058ac220b7b2e0a026";
		pfc::string8 xmlfile = http_get_xml(url, p_abort);
		XMLNode root = XMLNode::parseString(xmlfile, "lfm");
		XMLNode xml_list = root.getChildNode("toptracks");
		t_size page_number_of_tracks = xml_list.nChildNode("track");
		int i = 0;

		for (t_size page_track_iter = 0; page_track_iter < page_number_of_tracks && track_iter < spl_tracks_feed_limit; page_track_iter++, track_iter++) {
			XMLNode track = xml_list.getChildNode("track", &i);

			if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
				XMLNode artist = track.getChildNode("artist");

				if (artist.nChildNode("name") == 1) {
					track_titles.add_item(track.getChildNode("name").getText());
					track_artists.add_item(artist.getChildNode("name").getText());
				}
			}
		}
	}

	return(1);
}

bool download_user_friends_feed(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& users, abort_callback& p_abort)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=user.getfriends&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&limit=" << spl_friends_neighbours_feed_limit << "&api_key=b25b959554ed76058ac220b7b2e0a026";
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Friends feed for user '%s': possibly wrong username", lastfm_user.get_ptr());
		return(0);
	}

	XMLNode xml_list = root.getChildNode("friends");
	t_size number_of_users = xml_list.nChildNode("user");

	if (spl_friends_neighbours_feed_limit < number_of_users) {
		number_of_users = spl_friends_neighbours_feed_limit;
	}

	int i = 0;

	for (t_size user_iter = 0; user_iter < number_of_users; user_iter++) {
		XMLNode user = xml_list.getChildNode("user", &i);

		if (user.nChildNode("name") == 1) {
			users.add_item(user.getChildNode("name").getText());
		}
	}

	return(1);
}

bool download_user_neighbours_feed(pfc::string8 lastfm_user, pfc::list_t<pfc::string8>& users, abort_callback& p_abort)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=user.getneighbours&user=" << stringcvt_escape_url(lastfm_user).get_ptr() << "&limit=" << spl_friends_neighbours_feed_limit << "&api_key=b25b959554ed76058ac220b7b2e0a026";
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Neighbours feed for user '%s': possibly wrong username", lastfm_user.get_ptr());
		return(0);
	}

	XMLNode xml_list = root.getChildNode("neighbours");
	t_size number_of_users = xml_list.nChildNode("user");

	if (spl_friends_neighbours_feed_limit < number_of_users) {
		number_of_users = spl_friends_neighbours_feed_limit;
	}

	int i = 0;

	for (t_size user_iter = 0; user_iter < number_of_users; user_iter++) {
		XMLNode user = xml_list.getChildNode("user", &i);

		if (user.nChildNode("name") == 1) {
			users.add_item(user.getChildNode("name").getText());
		}
	}

	return(1);
}

bool download_artist_similar_artists_feed(pfc::string8 artist_name, pfc::list_t<pfc::string8>& artists, abort_callback& p_abort)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=artist.getsimilar&artist=" << stringcvt_escape_url(artist_name).get_ptr() << "&limit=" << spl_similar_artists_feed_limit << "&api_key=b25b959554ed76058ac220b7b2e0a026";
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Similar Artists feed for artist '%s': possibly unknown artist name", artist_name.get_ptr());
		return(0);
	}

	XMLNode xml_list = root.getChildNode("similarartists");
	t_size number_of_artists = xml_list.nChildNode("artist");

	if (spl_similar_artists_feed_limit < number_of_artists) {
		number_of_artists = spl_similar_artists_feed_limit;
	}

	int i = 0;

	for (t_size artist_iter = 0; artist_iter < number_of_artists; artist_iter++) {
		XMLNode artist = xml_list.getChildNode("artist", &i);

		if (artist.nChildNode("name") == 1) {
			artists.add_item(artist.getChildNode("name").getText());
		}
	}

	return(1);
}

bool download_similar_tracks_feed(pfc::string8 track_title, pfc::string8& track_artist, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=track.getsimilar&track=" << stringcvt_escape_url(track_title).get_ptr() << "&artist=" << stringcvt_escape_url(track_artist).get_ptr() << "&limit=" << spl_similar_tracks_feed_limit << "&api_key=b25b959554ed76058ac220b7b2e0a026";
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Similar Tracks feed for track '%s' by '%s': possibly unknown track name artist combination", track_title.get_ptr(), track_artist.get_ptr());
		return(0);
	}

	XMLNode xml_list = root.getChildNode("similartracks");
	t_size number_of_tracks = xml_list.nChildNode("track");

	if (spl_similar_tracks_feed_limit < number_of_tracks) {
		number_of_tracks = spl_similar_tracks_feed_limit;
	}

	int i = 0;

	for (t_size track_iter = 0; track_iter < number_of_tracks; track_iter++) {
		XMLNode track = xml_list.getChildNode("track", &i);

		if (track.nChildNode("name") == 1 && track.nChildNode("artist") == 1) {
			XMLNode artist = track.getChildNode("artist");

			if (artist.nChildNode("name") == 1) {
				track_titles.add_item(track.getChildNode("name").getText());
				track_artists.add_item(artist.getChildNode("name").getText());
			}
		}
	}

	return(1);
}

bool download_top_tags_feed(pfc::string8& tags)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=tag.gettoptags&api_key=b25b959554ed76058ac220b7b2e0a026";
	foobar2000_io::abort_callback_dummy p_abort;
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Top Tags feed");
		return(0);
	}

	XMLNode xml_list = root.getChildNode("toptags");
	t_size number_of_tags = xml_list.nChildNode("tag");

	if (spl_top_tags_feed_limit < number_of_tags) {
		number_of_tags = spl_top_tags_feed_limit;
	}

	int i = 0;
	tags = "";

	if (0 < number_of_tags) {
		XMLNode tag = xml_list.getChildNode("tag", &i);

		if (tag.nChildNode("name") == 1) {
			tags << tag.getChildNode("name").getText();
		}
	}

	for (t_size tag_iter = 1; tag_iter < number_of_tags; tag_iter++) {
		XMLNode tag = xml_list.getChildNode("tag", &i);

		if (tag.nChildNode("name") == 1) {
			tags << "|" << tag.getChildNode("name").getText();
		}
	}

	return(1);
}

bool download_playlists_feed(pfc::string8& playlist_ids, pfc::string8& playlist_names)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=user.getplaylists&user=" << stringcvt_escape_url(spl_lastfm_user).get_ptr() << "&api_key=b25b959554ed76058ac220b7b2e0a026";
	foobar2000_io::abort_callback_dummy p_abort;
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Playlists feed for user '%s': possibly wrong username", spl_lastfm_user.get_ptr());
		return(0);
	}

	XMLNode xml_list = root.getChildNode("playlists");
	t_size number_of_playlists = xml_list.nChildNode("playlist");
	playlist_ids = "";
	playlist_names = "";

	if (0 < number_of_playlists) {
		XMLNode playlist = xml_list.getChildNode("playlist", number_of_playlists - 1);

		if (playlist.nChildNode("id") == 1 && playlist.nChildNode("title") == 1) {
			playlist_ids << playlist.getChildNode("id").getText();
			playlist_names << playlist.getChildNode("title").getText();
		}
	}

	for (t_size tag_iter = 1; tag_iter < number_of_playlists; tag_iter++) {
		XMLNode playlist = xml_list.getChildNode("playlist", number_of_playlists - 1 - tag_iter);

		if (playlist.nChildNode("id") == 1 && playlist.nChildNode("title") == 1) {
			playlist_ids << "|" << playlist.getChildNode("id").getText();
			playlist_names << "|" << playlist.getChildNode("title").getText();
		}
	}

	return(1);
}

bool download_playlist_tracks_feed(pfc::string8 playlist_id, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, abort_callback& p_abort)
{
	pfc::string8 url;
	url << "http://ws.audioscrobbler.com/2.0/?method=playlist.fetch&playlistURL=lastfm://playlist/" << stringcvt_escape_url(playlist_id).get_ptr() << "&api_key=b25b959554ed76058ac220b7b2e0a026";
	pfc::string8 xmlfile = http_get_xml(url, p_abort);
	XMLNode root = XMLNode::parseString(xmlfile, "lfm");

	if (root.isEmpty()) {
		console::printf("foo_softplaylists: failed to download Last.fm Playlist feed");
		return(0);
	}

	XMLNode root2 = root.getChildNode("playlist");
	XMLNode xml_list = root2.getChildNode("trackList");
	t_size number_of_tracks = xml_list.nChildNode("track");

	if (spl_tracks_feed_limit < number_of_tracks) {
		number_of_tracks = spl_tracks_feed_limit;
	}

	int i = 0;

	for (t_size track_iter = 0; track_iter < number_of_tracks; track_iter++) {
		XMLNode track = xml_list.getChildNode("track", &i);

		if (track.nChildNode("title") == 1 && track.nChildNode("creator") == 1) {
			track_titles.add_item(track.getChildNode("title").getText());
			track_artists.add_item(track.getChildNode("creator").getText());
		}
	}

	return(1);
}




// matching

void sort_library_extract_lib_track_info(pfc::list_t<metadb_handle_ptr>& library, pfc::list_t<pfc::string8>& lib_track_titles, pfc::list_t<pfc::string8>& lib_track_artists, pfc::list_t<pfc::string8>& lib_track_paths, pfc::list_t<t_size>& lib_track_lengths, pfc::list_t<double>& lib_track_bitrates, pfc::list_t<pfc::string8>& lib_track_albums, pfc::list_t<pfc::string8>& lib_track_dates, pfc::list_t<t_size>& lib_track_trnums)
{
	pfc::list_t<metadb_handle_ptr> tmp_library;
	pfc::list_t<pfc::string8> tmp_lib_track_titles;
	pfc::list_t<pfc::string8> tmp_lib_track_artists;
	pfc::list_t<pfc::string8> tmp_lib_track_paths;
	pfc::list_t<t_size> tmp_lib_track_lengths;
	pfc::list_t<double> tmp_lib_track_bitrates;
	pfc::list_t<pfc::string8> tmp_lib_track_albums;
	pfc::list_t<pfc::string8> tmp_lib_track_dates;
	pfc::list_t<t_size> tmp_lib_track_trnums;
	{
		in_metadb_sync sync;

		for (t_size i = 0; i < library.get_count(); i++) {
			const file_info* fi;

			if (library[i]->get_info_locked(fi)) {
				tmp_library.add_item(library[i]);

				if (fi->meta_exists("title")) {
					tmp_lib_track_titles.add_item(fi->meta_get("title", 0));
				} else {
					tmp_lib_track_titles.add_item("title tag not present");
				}

				if (fi->meta_exists("artist")) {
					tmp_lib_track_artists.add_item(fi->meta_get("artist", 0));
				} else {
					tmp_lib_track_artists.add_item("artist tag not present");
				}

				tmp_lib_track_paths.add_item(library[i]->get_path());
				tmp_lib_track_lengths.add_item((t_size)(library[i]->get_length() * 1000));
				tmp_lib_track_bitrates.add_item((unsigned)floor((double)library[i]->get_filesize() * 8 / (library[i]->get_length() * 1000) + 0.5));

				if (fi->meta_exists("album")) {
					tmp_lib_track_albums.add_item(fi->meta_get("album", 0));
				} else {
					tmp_lib_track_albums.add_item("album tag not present");
				}

				if (fi->meta_exists("date")) {
					tmp_lib_track_dates.add_item(fi->meta_get("date", 0));
				} else {
					tmp_lib_track_dates.add_item("date tag not present");
				}

				if (fi->meta_exists("tracknumber")) {
					tmp_lib_track_trnums.add_item(atoi(fi->meta_get("tracknumber", 0)));
				} else {
					tmp_lib_track_trnums.add_item(pfc::infinite_size);
				}
			}
		}
	}
	pfc::list_t<t_size> order;
	order.set_size(tmp_library.get_count());
	order_helper::g_fill(order.get_ptr(), order.get_count());

	if (spl_partial_title) {
		tmp_lib_track_artists.sort_stable_get_permutation_t(stricmp_utf8, order.get_ptr());
	} else {
		tmp_lib_track_titles.sort_stable_get_permutation_t(stricmp_utf8, order.get_ptr());
	}

	library.remove_all();

	for (t_size i = 0; i < order.get_count(); i++) {
		library.add_item(tmp_library[order[i]]);
		lib_track_titles.add_item(tmp_lib_track_titles[order[i]]);
		lib_track_artists.add_item(tmp_lib_track_artists[order[i]]);
		lib_track_paths.add_item(tmp_lib_track_paths[order[i]]);
		lib_track_lengths.add_item(tmp_lib_track_lengths[order[i]]);
		lib_track_bitrates.add_item(tmp_lib_track_bitrates[order[i]]);
		lib_track_albums.add_item(tmp_lib_track_albums[order[i]]);
		lib_track_dates.add_item(tmp_lib_track_dates[order[i]]);
		lib_track_trnums.add_item(tmp_lib_track_trnums[order[i]]);
	}
}

metadb_handle_ptr get_preferred_track_by_info(pfc::string8 title, pfc::string8 artist, t_size length, pfc::string8 album, pfc::string8 date, t_size trnum, pfc::list_t<metadb_handle_ptr>& library, pfc::list_t<pfc::string8>& lib_track_titles, pfc::list_t<pfc::string8>& lib_track_artists, pfc::list_t<pfc::string8>& lib_track_paths, pfc::list_t<t_size>& lib_track_lengths, pfc::list_t<double>& lib_track_bitrates, pfc::list_t<pfc::string8>& lib_track_albums, pfc::list_t<pfc::string8>& lib_track_dates, pfc::list_t<t_size>& lib_track_trnums)
{
	t_size p_range_base;
	t_size p_range_count;

	if (spl_partial_title) {
		if (!pfc::bsearch_range_t(lib_track_artists.get_count(), lib_track_artists, stricmp_utf8, artist, p_range_base, p_range_count)) {
			if (spl_unmatched_to_console) {
				console::printf("foo_softplaylists: unmatched: '%s' - '%s' - '%s'", artist.get_ptr(), album.get_ptr(), title.get_ptr());
			}

			return(0);
		}
	} else {
		if (!pfc::bsearch_range_t(lib_track_titles.get_count(), lib_track_titles, stricmp_utf8, title, p_range_base, p_range_count)) {
			if (spl_unmatched_to_console) {
				console::printf("foo_softplaylists: unmatched: '%s' - '%s' - '%s'", artist.get_ptr(), album.get_ptr(), title.get_ptr());
			}

			return(0);
		}
	}

	pfc::list_t<metadb_handle_ptr> pref_match_library;
	pfc::list_t<pfc::string8> pref_match_track_titles_artists;
	pfc::list_t<t_size> pref_match_track_lengths;
	pfc::list_t<double> pref_match_track_bitrates;
	pfc::list_t<pfc::string8> pref_match_track_albums;
	pfc::list_t<pfc::string8> pref_match_track_dates;
	pfc::list_t<t_size> pref_match_track_trnums;
	pfc::list_t<metadb_handle_ptr> norm_match_library;
	pfc::list_t<pfc::string8> norm_match_track_titles_artists;
	pfc::list_t<t_size> norm_match_track_lengths;
	pfc::list_t<double> norm_match_track_bitrates;
	pfc::list_t<pfc::string8> norm_match_track_albums;
	pfc::list_t<pfc::string8> norm_match_track_dates;
	pfc::list_t<t_size> norm_match_track_trnums;

	for (t_size i = p_range_base; i < p_range_base + p_range_count; i++) {
		if (stricmp_utf8_partial(lib_track_paths[i], spl_path_preference) == 0) {
			pref_match_library.add_item(library[i]);

			if (spl_partial_title) {
				pref_match_track_titles_artists.add_item(lib_track_titles[i]);
			} else {
				pref_match_track_titles_artists.add_item(lib_track_artists[i]);
			}

			pref_match_track_lengths.add_item(lib_track_lengths[i]);
			pref_match_track_bitrates.add_item(lib_track_bitrates[i]);
			pref_match_track_albums.add_item(lib_track_albums[i]);
			pref_match_track_dates.add_item(lib_track_dates[i]);
			pref_match_track_trnums.add_item(lib_track_trnums[i]);
		} else {
			norm_match_library.add_item(library[i]);

			if (spl_partial_title) {
				norm_match_track_titles_artists.add_item(lib_track_titles[i]);
			} else {
				norm_match_track_titles_artists.add_item(lib_track_artists[i]);
			}

			norm_match_track_lengths.add_item(lib_track_lengths[i]);
			norm_match_track_bitrates.add_item(lib_track_bitrates[i]);
			norm_match_track_albums.add_item(lib_track_albums[i]);
			norm_match_track_dates.add_item(lib_track_dates[i]);
			norm_match_track_trnums.add_item(lib_track_trnums[i]);
		}
	}

	pfc::list_t<t_size> pref_order;
	pref_order.set_size(pref_match_library.get_count());
	order_helper::g_fill(pref_order.get_ptr(), pref_order.get_count());

	if (spl_track_preference_selection == 1 || spl_track_preference_selection == 2) {
		pref_match_track_bitrates.sort_stable_get_permutation_t(pfc::compare_t<double, double>, pref_order.get_ptr());
	}

	if (spl_track_preference_selection == 3 || spl_track_preference_selection == 4) {
		pref_match_track_lengths.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, pref_order.get_ptr());
	}

	if (spl_track_preference_selection == 5 || spl_track_preference_selection == 6) {
		pref_match_track_dates.sort_stable_get_permutation_t(stricmp_utf8, pref_order.get_ptr());
	}

	pfc::list_t<t_size> norm_order;
	norm_order.set_size(norm_match_library.get_count());
	order_helper::g_fill(norm_order.get_ptr(), norm_order.get_count());

	if (spl_track_preference_selection == 1 || spl_track_preference_selection == 2) {
		norm_match_track_bitrates.sort_stable_get_permutation_t(pfc::compare_t<double, double>, norm_order.get_ptr());
	}

	if (spl_track_preference_selection == 3 || spl_track_preference_selection == 4) {
		norm_match_track_lengths.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, norm_order.get_ptr());
	}

	if (spl_track_preference_selection == 5 || spl_track_preference_selection == 6) {
		norm_match_track_dates.sort_stable_get_permutation_t(stricmp_utf8, norm_order.get_ptr());
	}

	pfc::list_t<metadb_handle_ptr> match_library;
	pfc::list_t<pfc::string8> match_track_titles_artists;
	pfc::list_t<t_size> match_track_lengths;
	pfc::list_t<double> match_track_bitrates;
	pfc::list_t<pfc::string8> match_track_albums;
	pfc::list_t<pfc::string8> match_track_dates;
	pfc::list_t<t_size> match_track_trnums;

	if (spl_track_preference_selection == 1 || spl_track_preference_selection == 3 || spl_track_preference_selection == 5) {
		t_size perf_order_get_count_m1 = pref_order.get_count() - 1;

		for (t_size i = 0; i < pref_order.get_count(); i++) {
			match_library.add_item(pref_match_library[pref_order[perf_order_get_count_m1 - i]]);
			match_track_titles_artists.add_item(pref_match_track_titles_artists[pref_order[perf_order_get_count_m1 - i]]);
			match_track_lengths.add_item(pref_match_track_lengths[pref_order[perf_order_get_count_m1 - i]]);
			match_track_bitrates.add_item(pref_match_track_bitrates[pref_order[perf_order_get_count_m1 - i]]);
			match_track_albums.add_item(pref_match_track_albums[pref_order[perf_order_get_count_m1 - i]]);
			match_track_dates.add_item(pref_match_track_dates[pref_order[perf_order_get_count_m1 - i]]);
			match_track_trnums.add_item(pref_match_track_trnums[pref_order[perf_order_get_count_m1 - i]]);
		}

		t_size norm_order_get_count_m1 = norm_order.get_count() - 1;

		for (t_size i = 0; i < norm_order.get_count(); i++) {
			match_library.add_item(norm_match_library[norm_order[norm_order_get_count_m1 - i]]);
			match_track_titles_artists.add_item(norm_match_track_titles_artists[norm_order[norm_order_get_count_m1 - i]]);
			match_track_lengths.add_item(norm_match_track_lengths[norm_order[norm_order_get_count_m1 - i]]);
			match_track_bitrates.add_item(norm_match_track_bitrates[norm_order[norm_order_get_count_m1 - i]]);
			match_track_albums.add_item(norm_match_track_albums[norm_order[norm_order_get_count_m1 - i]]);
			match_track_dates.add_item(norm_match_track_dates[norm_order[norm_order_get_count_m1 - i]]);
			match_track_trnums.add_item(norm_match_track_trnums[norm_order[norm_order_get_count_m1 - i]]);
		}
	}

	if (spl_track_preference_selection == 2 || spl_track_preference_selection == 4 || spl_track_preference_selection == 6) {
		for (t_size i = 0; i < pref_order.get_count(); i++) {
			match_library.add_item(pref_match_library[pref_order[i]]);
			match_track_titles_artists.add_item(pref_match_track_titles_artists[pref_order[i]]);
			match_track_lengths.add_item(pref_match_track_lengths[pref_order[i]]);
			match_track_bitrates.add_item(pref_match_track_bitrates[pref_order[i]]);
			match_track_albums.add_item(pref_match_track_albums[pref_order[i]]);
			match_track_dates.add_item(pref_match_track_dates[pref_order[i]]);
			match_track_trnums.add_item(pref_match_track_trnums[pref_order[i]]);
		}

		for (t_size i = 0; i < norm_order.get_count(); i++) {
			match_library.add_item(norm_match_library[norm_order[i]]);
			match_track_titles_artists.add_item(norm_match_track_titles_artists[norm_order[i]]);
			match_track_lengths.add_item(norm_match_track_lengths[norm_order[i]]);
			match_track_bitrates.add_item(norm_match_track_bitrates[norm_order[i]]);
			match_track_albums.add_item(norm_match_track_albums[norm_order[i]]);
			match_track_dates.add_item(norm_match_track_dates[norm_order[i]]);
			match_track_trnums.add_item(norm_match_track_trnums[norm_order[i]]);
		}
	}

	bool match = 0;
	t_size match_title_artist_pos = pfc::infinite_size;
	t_size match_album_pos = pfc::infinite_size;
	t_size match_length_pos = pfc::infinite_size;
	t_size i = 0;

	while (!match && i < match_library.get_count()) {
		bool match_title_artist;

		if (spl_partial_title) {
			match_title_artist = (stricmp_utf8_partial(title, match_track_titles_artists[i]) == 0 || stricmp_utf8_partial(match_track_titles_artists[i], title) == 0);
		} else {
			match_title_artist = (stricmp_utf8(match_track_titles_artists[i], artist) == 0);
		}

		if (match_title_artist && match_title_artist_pos == pfc::infinite_size) {
			match_title_artist_pos = i;
		}

		bool match_album = (match_title_artist && (stricmp_utf8(match_track_albums[i], album) == 0));

		if (match_album && match_album_pos == pfc::infinite_size) {
			match_album_pos = i;
		}

		bool match_length = (match_album && (match_track_lengths[i] == length));

		if (match_length && match_length_pos == pfc::infinite_size) {
			match_length_pos = i;
		}

		match = (match_length && (match_track_trnums[i] == trnum));
		i++;
	}

	metadb_handle_ptr track = 0;

	if (match) {
		track = match_library[--i];
	} else if (match_length_pos != pfc::infinite_size) {
		track = match_library[match_length_pos];
	} else if (match_album_pos != pfc::infinite_size) {
		track = match_library[match_album_pos];
	} else if (match_title_artist_pos != pfc::infinite_size) {
		track = match_library[match_title_artist_pos];
	}

	if (spl_unmatched_to_console) {
		if (track == 0) {
			console::printf("foo_softplaylists: unmatched: '%s' - '%s' - '%s'", artist.get_ptr(), album.get_ptr(), title.get_ptr());
		}
	}

	return(track);
}




// get track info

bool get_artist(metadb_handle_list_cref p_data, pfc::string8& track_artist)
{
	if (p_data.get_count() == 0) {
		return(0);
	}

	file_info_impl fi;

	if (p_data[0]->get_info(fi) && fi.meta_exists("artist")) {
		track_artist = fi.meta_get("artist", 0);
		return(1);
	} else {
		return(0);
	}
}

bool get_track_title_artist(metadb_handle_list_cref p_data, pfc::string8& track_title, pfc::string8& track_artist)
{
	if (p_data.get_count() == 0) {
		return(0);
	}

	file_info_impl fi;

	if (p_data[0]->get_info(fi) && fi.meta_exists("title") && fi.meta_exists("artist")) {
		track_title = fi.meta_get("title", 0);
		track_artist = fi.meta_get("artist", 0);
		return(1);
	} else {
		return(0);
	}
}




// last.fm playlist generation main menu

class GenerateUserTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::string8 period;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<t_size> playcounts;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<t_size> track_playcounts;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateUserTopTracksPlaylist(pfc::string8 user_name, pfc::string8 period) {
		this->user_name = user_name;
		this->period = period;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Top Tracks feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_top_tracks_and_playcounts_feed_pages(user_name, period, track_titles, track_artists, track_playcounts, p_abort);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				bool match = 0;

				for (t_size i = 0; i < tracks.get_count(); i++) {
					if (track == tracks[i]) {
						playcounts[i] += track_playcounts[track_iter];
						match = 1;
						break;
					}
				}

				if (!match) {
					tracks.add_item(track);
					playcounts.add_item(track_playcounts[track_iter]);
				}
			}
		}

		p_status.set_item("Sorting Tracks By Playcount ...");
		p_status.force_update();
		pfc::list_t<t_size> order;
		order.set_size(tracks.get_count());
		order_helper::g_fill(order.get_ptr(), order.get_count());
		playcounts.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, order.get_ptr());
		pfc::list_t<metadb_handle_ptr> tmp_tracks = tracks;
		t_size order_get_count_m1 = order.get_count() - 1;

		for (t_size i = 0; i < order.get_count(); i++) {
			tracks[i] = tmp_tracks[order[order_get_count_m1 - i]];
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateNeighboursTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::string8 period;
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<t_size> playcounts;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<t_size> track_playcounts;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateNeighboursTopTracksPlaylist(pfc::string8 user_name, pfc::string8 period) {
		this->user_name = user_name;
		this->period = period;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Neighbours feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_neighbours_feed(user_name, users, p_abort);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Top Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_top_tracks_and_playcounts_feed_pages(user, period, track_titles, track_artists, track_playcounts, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					bool match = 0;

					for (t_size i = 0; i < tracks.get_count(); i++) {
						if (track == tracks[i]) {
							playcounts[i] += track_playcounts[track_iter];
							match = 1;
							break;
						}
					}

					if (!match) {
						tracks.add_item(track);
						playcounts.add_item(track_playcounts[track_iter]);
					}
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();
			track_playcounts.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Sorting Tracks By Playcount ...");
		p_status.force_update();
		pfc::list_t<t_size> order;
		order.set_size(tracks.get_count());
		order_helper::g_fill(order.get_ptr(), order.get_count());
		playcounts.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, order.get_ptr());
		pfc::list_t<metadb_handle_ptr> tmp_tracks = tracks;
		t_size order_get_count_m1 = order.get_count() - 1;

		for (t_size i = 0; i < order.get_count(); i++) {
			tracks[i] = tmp_tracks[order[order_get_count_m1 - i]];
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Neighbours Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateFriendsTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::string8 period;
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<t_size> playcounts;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<t_size> track_playcounts;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateFriendsTopTracksPlaylist(pfc::string8 user_name, pfc::string8 period) {
		this->user_name = user_name;
		this->period = period;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Friends feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_friends_feed(user_name, users, p_abort);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Top Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_top_tracks_and_playcounts_feed_pages(user, period, track_titles, track_artists, track_playcounts, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					bool match = 0;

					for (t_size i = 0; i < tracks.get_count(); i++) {
						if (track == tracks[i]) {
							playcounts[i] += track_playcounts[track_iter];
							match = 1;
							break;
						}
					}

					if (!match) {
						tracks.add_item(track);
						playcounts.add_item(track_playcounts[track_iter]);
					}
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();
			track_playcounts.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Sorting Tracks By Playcount ...");
		p_status.force_update();
		pfc::list_t<t_size> order;
		order.set_size(tracks.get_count());
		order_helper::g_fill(order.get_ptr(), order.get_count());
		playcounts.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, order.get_ptr());
		pfc::list_t<metadb_handle_ptr> tmp_tracks = tracks;
		t_size order_get_count_m1 = order.get_count() - 1;

		for (t_size i = 0; i < order.get_count(); i++) {
			tracks[i] = tmp_tracks[order[order_get_count_m1 - i]];
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Friends Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateCustomUsersTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::list_t<pfc::string8> users;
	pfc::string8 period;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<t_size> playcounts;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<t_size> track_playcounts;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateCustomUsersTopTracksPlaylist(pfc::list_t<pfc::string8> users, pfc::string8 period) {
		this->users = users;
		this->period = period;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Top Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_top_tracks_and_playcounts_feed_pages(user, period, track_titles, track_artists, track_playcounts, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					bool match = 0;

					for (t_size i = 0; i < tracks.get_count(); i++) {
						if (track == tracks[i]) {
							playcounts[i] += track_playcounts[track_iter];
							match = 1;
							break;
						}
					}

					if (!match) {
						tracks.add_item(track);
						playcounts.add_item(track_playcounts[track_iter]);
					}
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();
			track_playcounts.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Sorting Tracks By Playcount ...");
		p_status.force_update();
		pfc::list_t<t_size> order;
		order.set_size(tracks.get_count());
		order_helper::g_fill(order.get_ptr(), order.get_count());
		playcounts.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, order.get_ptr());
		pfc::list_t<metadb_handle_ptr> tmp_tracks = tracks;
		t_size order_get_count_m1 = order.get_count() - 1;

		for (t_size i = 0; i < order.get_count(); i++) {
			tracks[i] = tmp_tracks[order[order_get_count_m1 - i]];
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "Custom Users Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateTagTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 tag_name;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateTagTopTracksPlaylist(pfc::string8 tag_name) {
		this->tag_name = tag_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Top Tracks feed for tag '" << tag_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_tag_top_tracks_feed_pages(tag_name, track_titles, track_artists, p_abort);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				tracks.add_item(track);
			}
		}

		p_status.set_item("Removing Duplicate Tracks ...");
		p_status.force_update();
		metadb_handle_list_helper::remove_duplicates(tracks);
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << tag_name << "' Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateUserLovedTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateUserLovedTracksPlaylist(pfc::string8 user_name) {
		this->user_name = user_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Loved Tracks feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_loved_tracks_feed_pages(user_name, track_titles, track_artists, p_abort);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				tracks.add_item(track);
			}
		}

		p_status.set_item("Removing Duplicate Tracks ...");
		p_status.force_update();
		metadb_handle_list_helper::remove_duplicates(tracks);
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Loved Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateNeighboursLovedTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateNeighboursLovedTracksPlaylist(pfc::string8 user_name) {
		this->user_name = user_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Neighbours feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_neighbours_feed(user_name, users, p_abort);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Loved Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_loved_tracks_feed_pages(user, track_titles, track_artists, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					tracks.add_item(track);
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Removing Duplicate Tracks ...");
		p_status.force_update();
		metadb_handle_list_helper::remove_duplicates(tracks);
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Neighbours Loved Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateFriendsLovedTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateFriendsLovedTracksPlaylist(pfc::string8 user_name) {
		this->user_name = user_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Friends feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_friends_feed(user_name, users, p_abort);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Loved Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_loved_tracks_feed_pages(user, track_titles, track_artists, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					tracks.add_item(track);
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Removing Duplicate Tracks ...");
		p_status.force_update();
		metadb_handle_list_helper::remove_duplicates(tracks);
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Friends Loved Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateCustomUsersLovedTracksPlaylist : public threaded_process_callback {
private:
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateCustomUsersLovedTracksPlaylist(pfc::list_t<pfc::string8> users) {
		this->users = users;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Loved Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_loved_tracks_feed_pages(user, track_titles, track_artists, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					tracks.add_item(track);
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Removing Duplicate Tracks ...");
		p_status.force_update();
		metadb_handle_list_helper::remove_duplicates(tracks);
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "Custom Users Loved Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateUserRecentTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateUserRecentTracksPlaylist(pfc::string8 user_name) {
		this->user_name = user_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Recent Tracks feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_recent_tracks_feed_pages(user_name, track_titles, track_artists, p_abort);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				tracks.add_item(track);
			}
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Recent Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateNeighboursRecentTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateNeighboursRecentTracksPlaylist(pfc::string8 user_name) {
		this->user_name = user_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Neighbours feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_neighbours_feed(user_name, users, p_abort);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Recent Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_recent_tracks_feed_pages(user, track_titles, track_artists, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					tracks.add_item(track);
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Neighbours Recent Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateFriendsRecentTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 user_name;
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateFriendsRecentTracksPlaylist(pfc::string8 user_name) {
		this->user_name = user_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Friends feed for user '" << user_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_user_friends_feed(user_name, users, p_abort);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Recent Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_recent_tracks_feed_pages(user, track_titles, track_artists, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					tracks.add_item(track);
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << user_name << "' Friends Recent Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateCustomUsersRecentTracksPlaylist : public threaded_process_callback {
private:
	pfc::list_t<pfc::string8> users;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateCustomUsersRecentTracksPlaylist(pfc::list_t<pfc::string8> users) {
		this->users = users;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

		for (t_uint32 user_iter = 0; user_iter < users.get_count(); user_iter++) {
			pfc::string8 user = users[user_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << user_iter + 1 << "/" << users.get_count() << ": Downloading Last.fm Recent Tracks feed for user '" << user << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_user_recent_tracks_feed_pages(user, track_titles, track_artists, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << user_iter + 1 << "/" << users.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					tracks.add_item(track);
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "Custom Users Recent Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};




// last.fm playlist generation context menu

class GenerateArtistTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 artist_name;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<t_size> playcounts;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<t_size> track_playcounts;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateArtistTopTracksPlaylist(pfc::string8 artist_name) {
		this->artist_name = artist_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Top Tracks feed for artist '" << artist_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_artist_top_tracks_and_playcounts_feed_pages(artist_name, track_titles, track_artists, track_playcounts, p_abort);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				bool match = 0;

				for (t_size i = 0; i < tracks.get_count(); i++) {
					if (track == tracks[i]) {
						playcounts[i] += track_playcounts[track_iter];
						match = 1;
						break;
					}
				}

				if (!match) {
					tracks.add_item(track);
					playcounts.add_item(track_playcounts[track_iter]);
				}
			}
		}

		p_status.set_item("Sorting Tracks By Playcount ...");
		p_status.force_update();
		pfc::list_t<t_size> order;
		order.set_size(tracks.get_count());
		order_helper::g_fill(order.get_ptr(), order.get_count());
		playcounts.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, order.get_ptr());
		pfc::list_t<metadb_handle_ptr> tmp_tracks = tracks;
		t_size order_get_count_m1 = order.get_count() - 1;

		for (t_size i = 0; i < order.get_count(); i++) {
			tracks[i] = tmp_tracks[order[order_get_count_m1 - i]];
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << artist_name << "' Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateSimilarArtistsTopTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 artist_name;
	pfc::list_t<pfc::string8> artists;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<t_size> playcounts;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<t_size> track_playcounts;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateSimilarArtistsTopTracksPlaylist(pfc::string8 artist_name) {
		this->artist_name = artist_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Similar Artists feed for artist '" << artist_name << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_artist_similar_artists_feed(artist_name, artists, p_abort);

		for (t_uint32 artist_iter = 0; artist_iter < artists.get_count(); artist_iter++) {
			pfc::string8 artist = artists[artist_iter];
			pfc::string8 tmp_string1;
			tmp_string1 << artist_iter + 1 << "/" << artists.get_count() << ": Downloading Last.fm Top Tracks feed for artist '" << artist << "' ...";
			p_status.set_item(tmp_string1);
			p_status.force_update();
			download_artist_top_tracks_and_playcounts_feed_pages(artist, track_titles, track_artists, track_playcounts, p_abort);
			pfc::string8 tmp_string2;
			tmp_string2 << artist_iter + 1 << "/" << artists.get_count() << ": Matching Tracks ...";
			p_status.set_item(tmp_string2);
			p_status.force_update();

			for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
				metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

				if (track != 0) {
					bool match = 0;

					for (t_size i = 0; i < tracks.get_count(); i++) {
						if (track == tracks[i]) {
							playcounts[i] += track_playcounts[track_iter];
							match = 1;
							break;
						}
					}

					if (!match) {
						tracks.add_item(track);
						playcounts.add_item(track_playcounts[track_iter]);
					}
				}
			}

			track_titles.remove_all();
			track_artists.remove_all();
			track_playcounts.remove_all();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Sorting Tracks By Playcount ...");
		p_status.force_update();
		pfc::list_t<t_size> order;
		order.set_size(tracks.get_count());
		order_helper::g_fill(order.get_ptr(), order.get_count());
		playcounts.sort_stable_get_permutation_t(pfc::compare_t<t_size, t_size>, order.get_ptr());
		pfc::list_t<metadb_handle_ptr> tmp_tracks = tracks;
		t_size order_get_count_m1 = order.get_count() - 1;

		for (t_size i = 0; i < order.get_count(); i++) {
			tracks[i] = tmp_tracks[order[order_get_count_m1 - i]];
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << artist_name << "' Similar Artists Top Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateSimilarTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 track_title;
	pfc::string8 track_artist;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateSimilarTracksPlaylist(pfc::string8 track_title, pfc::string8 track_artist) {
		this->track_title = track_title;
		this->track_artist = track_artist;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Similar Tracks feed for track '" << track_title << "' by '" << track_artist << "' ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_similar_tracks_feed(track_title, track_artist, track_titles, track_artists, p_abort);
		t_uint32 lookup_start = 0;
		t_uint32 lookup_end = (t_uint32)track_titles.get_count();

		for (t_uint32 recursion_iter = 0; recursion_iter < spl_similar_tracks_recursion_limit; recursion_iter++) {
			for (t_uint32 lookup_iter = lookup_start; lookup_iter < lookup_end; lookup_iter++) {
				pfc::string8 tmp_string1;
				tmp_string1 << recursion_iter + 1 << "/" << spl_similar_tracks_recursion_limit << ":" << lookup_iter + 1 << "/" << lookup_end << ": Downloading Last.fm Similar Tracks feed for track '" << track_titles[lookup_iter] << "' by '" << track_artists[lookup_iter] << "' ...";
				p_status.set_item(tmp_string1);
				p_status.force_update();
				download_similar_tracks_feed(track_titles[lookup_iter], track_artists[lookup_iter], track_titles, track_artists, p_abort);

				if (p_abort.is_aborting()) {
					break;
				}
			}

			lookup_start = lookup_end;
			lookup_end = (t_uint32)track_titles.get_count();

			if (p_abort.is_aborting()) {
				break;
			}
		}

		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				tracks.add_item(track);
			}
		}

		p_status.set_item("Removing Duplicate Tracks ...");
		p_status.force_update();
		metadb_handle_list_helper::remove_duplicates(tracks);
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 playlist_name;
		pfc::string8 playlist_name_prefix;

		if (spl_prefix_playlists) {
			playlist_name_prefix << "lfm: ";
		} else {
			playlist_name_prefix << "";
		}

		playlist_name << playlist_name_prefix << "'" << track_title << "' By '" << track_artist << "' Level " << spl_similar_tracks_recursion_limit << " Similar Tracks";
		t_size playlist = pm->find_or_create_playlist(playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

class GenerateTracksPlaylist : public threaded_process_callback {
private:
	pfc::string8 playlist_id;
	pfc::string8 playlist_name;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	GenerateTracksPlaylist(pfc::string8 playlist_id, pfc::string8 playlist_name) {
		this->playlist_id = playlist_id;
		this->playlist_name = playlist_name;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		pfc::string8 tmp_string1;
		tmp_string1 << "Downloading Last.fm Playlist feed ...";
		p_status.set_item(tmp_string1);
		p_status.force_update();
		download_playlist_tracks_feed(playlist_id, track_titles, track_artists, p_abort);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], pfc::infinite_size, "", "", pfc::infinite_size, library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				tracks.add_item(track);
			}
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string8 foobar_playlist_name;
		pfc::string8 foobar_playlist_name_prefix;

		if (spl_prefix_playlists) {
			foobar_playlist_name_prefix << "lfm: ";
		} else {
			foobar_playlist_name_prefix << "";
		}

		foobar_playlist_name << foobar_playlist_name_prefix << playlist_name;
		t_size playlist = pm->find_or_create_playlist(foobar_playlist_name, pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};




// love, unlove, ban or unban a track on last.fm user account

bool api_method_track_title_artist(pfc::string8 api_method_name, pfc::string8 track_title, pfc::string8 track_artist, abort_callback& p_abort)
{
	pfc::string8 api_key("d4b90f3283000f05768db60c6acef75c");
	pfc::string8 api_secret("9d000dffdd0d7cf0b721eba6a5e169ad");
	pfc::string8 api_method_session("auth.getmobilesession");
	pfc::string8 authToken;
	authToken << spl_lastfm_user << spl_lastfm_password_md5;
	static_api_ptr_t<hasher_md5> hasher;
	hasher_md5_result hash = hasher->process_single(authToken, strlen(authToken));
	pfc::string8 authToken_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	pfc::string8 api_sig_session;
	api_sig_session << "api_key" << api_key;
	api_sig_session << "authToken" << authToken_md5;
	api_sig_session << "method" << api_method_session;
	api_sig_session << "username" << spl_lastfm_user;
	api_sig_session << api_secret;
	hash = hasher->process_single(api_sig_session, strlen(api_sig_session));
	pfc::string8 api_sig_session_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	pfc::string8 url_session;
	url_session << "http://ws.audioscrobbler.com/2.0/";
	url_session << "?method=" << api_method_session;
	url_session << "&username=" << spl_lastfm_user;
	url_session << "&authToken=" << authToken_md5;
	url_session << "&api_key=" << api_key;
	url_session << "&api_sig=" << api_sig_session_md5;
	pfc::string8 xmlfile_session = http_get_xml(url_session, p_abort);
	XMLNode root_session = XMLNode::parseString(xmlfile_session, "lfm");

	if (root_session.isEmpty()) {
		console::printf("foo_softplaylists: failed to %s track: authentication failed: empty reply", api_method_name.get_ptr());
		return(0);
	}

	if (_stricmp(root_session.getAttributeValue(), "ok") != 0) {
		console::printf("foo_softplaylists: failed to %s track: authentication failed: possibly wrong username/password", api_method_name.get_ptr());
		return(0);
	}

	XMLNode session = root_session.getChildNode("session");
	XMLNode key_node = session.getChildNode("key");
	pfc::string8 sk = key_node.getText();
	pfc::string8 api_method_post;
	api_method_post << "track." << api_method_name;
	pfc::string8 api_sig_post;
	api_sig_post << "api_key" << api_key;
	api_sig_post << "artist" << track_artist;
	api_sig_post << "method" << api_method_post;
	api_sig_post << "sk" << sk;
	api_sig_post << "track" << track_title;
	api_sig_post << api_secret;
	hash = hasher->process_single(api_sig_post, strlen(api_sig_post));
	pfc::string8 api_sig_post_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	http_client::ptr client;

	if (!service_enum_t<http_client>().first(client)) {
		console::printf("foo_softplaylists: unable to start http service");
		return(0);
	}

	http_request::ptr request = client->create_request("POST");
	http_request_post::ptr request_post;
	request->service_query_t(request_post);
	request_post->add_post_data("method", api_method_post.get_ptr());
	request_post->add_post_data("track", track_title.get_ptr());
	request_post->add_post_data("artist", track_artist.get_ptr());
	request_post->add_post_data("api_key", api_key.get_ptr());
	request_post->add_post_data("api_sig", api_sig_post_md5.get_ptr());
	request_post->add_post_data("sk", sk.get_ptr());
	file::ptr file_ptr = request_post->run_ex("http://ws.audioscrobbler.com/2.0/", p_abort);
	char buffer[1025];
	t_size bytes_read;
	pfc::string8 xmlfile_post;

	while (bytes_read = file_ptr->read(buffer, 1024, p_abort)) {
		xmlfile_post.add_string(buffer, bytes_read);
	}

	XMLNode root_post = XMLNode::parseString(xmlfile_post, "lfm");

	if (root_post.isEmpty()) {
		console::printf("foo_softplaylists: failed to %s track: empty reply", api_method_name.get_ptr());
		return(0);
	}

	if (_stricmp(root_post.getAttributeValue(), "ok") != 0) {
		if (root_post.nChildNode("error") == 1) {
			XMLNode error = root_post.getChildNode("error");
			console::printf("foo_softplaylists: failed to %s track: error: %s", api_method_name.get_ptr(), error.getText());
		} else {
			console::printf("foo_softplaylists: failed to %s track: error: unknown", api_method_name.get_ptr());
		}

		return(0);
	}

	return(1);
}




// add a track to a last.fm playlist

bool api_method_playlist_track_title_artist(pfc::string8 api_method_name, pfc::string8 playlist_id, pfc::string8 track_title, pfc::string8 track_artist, abort_callback& p_abort)
{
	pfc::string8 api_key("d4b90f3283000f05768db60c6acef75c");
	pfc::string8 api_secret("9d000dffdd0d7cf0b721eba6a5e169ad");
	pfc::string8 api_method_session("auth.getmobilesession");
	pfc::string8 authToken;
	authToken << spl_lastfm_user << spl_lastfm_password_md5;
	static_api_ptr_t<hasher_md5> hasher;
	hasher_md5_result hash = hasher->process_single(authToken, strlen(authToken));
	pfc::string8 authToken_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	pfc::string8 api_sig_session;
	api_sig_session << "api_key" << api_key;
	api_sig_session << "authToken" << authToken_md5;
	api_sig_session << "method" << api_method_session;
	api_sig_session << "username" << spl_lastfm_user;
	api_sig_session << api_secret;
	hash = hasher->process_single(api_sig_session, strlen(api_sig_session));
	pfc::string8 api_sig_session_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	pfc::string8 url_session;
	url_session << "http://ws.audioscrobbler.com/2.0/";
	url_session << "?method=" << api_method_session;
	url_session << "&username=" << spl_lastfm_user;
	url_session << "&authToken=" << authToken_md5;
	url_session << "&api_key=" << api_key;
	url_session << "&api_sig=" << api_sig_session_md5;
	pfc::string8 xmlfile_session = http_get_xml(url_session, p_abort);
	XMLNode root_session = XMLNode::parseString(xmlfile_session, "lfm");

	if (root_session.isEmpty()) {
		console::printf("foo_softplaylists: failed to %s: authentication failed: empty reply", api_method_name.get_ptr());
		return(0);
	}

	if (_stricmp(root_session.getAttributeValue(), "ok") != 0) {
		console::printf("foo_softplaylists: failed to %s: authentication failed: possibly wrong username/password", api_method_name.get_ptr());
		return(0);
	}

	XMLNode session = root_session.getChildNode("session");
	XMLNode key_node = session.getChildNode("key");
	pfc::string8 sk = key_node.getText();
	pfc::string8 api_method_post;
	api_method_post << "playlist." << api_method_name;
	pfc::string8 api_sig_post;
	api_sig_post << "api_key" << api_key;
	api_sig_post << "artist" << track_artist;
	api_sig_post << "method" << api_method_post;
	api_sig_post << "playlistID" << playlist_id;
	api_sig_post << "sk" << sk;
	api_sig_post << "track" << track_title;
	api_sig_post << api_secret;
	hash = hasher->process_single(api_sig_post, strlen(api_sig_post));
	pfc::string8 api_sig_post_md5 = pfc::format_hexdump_lowercase(hash.m_data, sizeof(hash.m_data), "");
	http_client::ptr client;

	if (!service_enum_t<http_client>().first(client)) {
		console::printf("foo_softplaylists: unable to start http service");
		return(0);
	}

	http_request::ptr request = client->create_request("POST");
	http_request_post::ptr request_post;
	request->service_query_t(request_post);
	request_post->add_post_data("method", api_method_post.get_ptr());
	request_post->add_post_data("playlistID", playlist_id.get_ptr());
	request_post->add_post_data("track", track_title.get_ptr());
	request_post->add_post_data("artist", track_artist.get_ptr());
	request_post->add_post_data("api_key", api_key.get_ptr());
	request_post->add_post_data("api_sig", api_sig_post_md5.get_ptr());
	request_post->add_post_data("sk", sk.get_ptr());
	file::ptr file_ptr = request_post->run_ex("http://ws.audioscrobbler.com/2.0/", p_abort);
	char buffer[1025];
	t_size bytes_read;
	pfc::string8 xmlfile_post;

	while (bytes_read = file_ptr->read(buffer, 1024, p_abort)) {
		xmlfile_post.add_string(buffer, bytes_read);
	}

	XMLNode root_post = XMLNode::parseString(xmlfile_post, "lfm");

	if (root_post.isEmpty()) {
		console::printf("foo_softplaylists: failed to %s: empty reply", api_method_name.get_ptr());
		return(0);
	}

	if (_stricmp(root_post.getAttributeValue(), "ok") != 0) {
		if (root_post.nChildNode("error") == 1) {
			XMLNode error = root_post.getChildNode("error");
			console::printf("foo_softplaylists: failed to %s: error: %s", api_method_name.get_ptr(), error.getText());
		} else {
			console::printf("foo_softplaylists: failed to %s: error: unknown", api_method_name.get_ptr());
		}

		return(0);
	}

	return(1);
}




// xspf

void load_xspf_playlist(pfc::string8& xspf_filepath, pfc::list_t<pfc::string8>& track_titles, pfc::list_t<pfc::string8>& track_artists, pfc::list_t<pfc::string8>& track_albums, pfc::list_t<t_size>& track_lengths, pfc::list_t<t_size>& track_trnums)
{
	pfc::string8 xmlfile;
	abort_callback_impl p_abort;
	bool is_utf8;
	text_file_loader::read(xspf_filepath, p_abort, xmlfile, is_utf8);
	XMLNode root = XMLNode::parseString(pfc::stringcvt::string_ansi_from_utf8(xmlfile).get_ptr(), "playlist");
	XMLNode xml_list = root.getChildNode("trackList");
	int number_of_tracks = xml_list.nChildNode("track");
	int i = 0;

	for (int track_iter = 0; track_iter < number_of_tracks; track_iter++) {
		XMLNode track = xml_list.getChildNode("track", &i);

		if (track.nChildNode("title") == 1) {
			XMLNode track_name = track.getChildNode("title");
			track_titles.add_item(track_name.getText());
		} else {
			track_titles.add_item("");
		}

		if (track.nChildNode("creator") == 1) {
			XMLNode artist_name = track.getChildNode("creator");
			track_artists.add_item(artist_name.getText());
		} else {
			track_artists.add_item("");
		}

		if (track.nChildNode("album") == 1) {
			XMLNode album_name = track.getChildNode("album");
			track_albums.add_item(album_name.getText());
		} else {
			track_albums.add_item("");
		}

		if (track.nChildNode("trackNum") == 1) {
			XMLNode trnum_name = track.getChildNode("trackNum");
			track_trnums.add_item(atoi(trnum_name.getText()));
		} else {
			track_trnums.add_item(pfc::infinite_size);
		}

		if (track.nChildNode("duration") == 1) {
			XMLNode length_name = track.getChildNode("duration");
			track_lengths.add_item(atoi(length_name.getText()));
		} else {
			track_lengths.add_item(pfc::infinite_size);
		}
	}
}

class LoadXSPFPlaylist : public threaded_process_callback {
private:
	pfc::string8 xspf_filepath;
	pfc::list_t<metadb_handle_ptr> library;
	pfc::list_t<metadb_handle_ptr> tracks;
	pfc::list_t<pfc::string8> track_titles;
	pfc::list_t<pfc::string8> track_artists;
	pfc::list_t<pfc::string8> track_albums;
	pfc::list_t<t_size> track_lengths;
	pfc::list_t<t_size> track_trnums;
	pfc::list_t<pfc::string8> lib_track_titles;
	pfc::list_t<pfc::string8> lib_track_artists;
	pfc::list_t<pfc::string8> lib_track_paths;
	pfc::list_t<t_size> lib_track_lengths;
	pfc::list_t<double> lib_track_bitrates;
	pfc::list_t<pfc::string8> lib_track_albums;
	pfc::list_t<pfc::string8> lib_track_dates;
	pfc::list_t<t_size> lib_track_trnums;
public:
	LoadXSPFPlaylist(pfc::string8 xspf_filepath) {
		this->xspf_filepath = xspf_filepath;
	}
	void on_init(HWND p_wnd) {
		static_api_ptr_t<library_manager> lm;
		lm->get_all_items(library);
	}
	void run(threaded_process_status& p_status, abort_callback& p_abort) {
		p_status.set_item("Querying Local Database ...");
		p_status.force_update();
		sort_library_extract_lib_track_info(library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);
		p_status.set_item("Loading XSPF Playlist ...");
		p_status.force_update();
		load_xspf_playlist(xspf_filepath, track_titles, track_artists, track_albums, track_lengths, track_trnums);
		p_status.set_item("Matching Tracks ...");
		p_status.force_update();

		for (t_size track_iter = 0; track_iter < track_titles.get_count(); track_iter++) {
			metadb_handle_ptr track = get_preferred_track_by_info(track_titles[track_iter], track_artists[track_iter], track_lengths[track_iter], track_albums[track_iter], "", track_trnums[track_iter], library, lib_track_titles, lib_track_artists, lib_track_paths, lib_track_lengths, lib_track_bitrates, lib_track_albums, lib_track_dates, lib_track_trnums);

			if (track != 0) {
				tracks.add_item(track);
			}
		}
	}
	void on_done(HWND p_wnd, bool p_was_aborted) {
		static_api_ptr_t<playlist_manager> pm;
		pfc::string playlist_name = pfc::io::path::getFileNameWithoutExtension(xspf_filepath);
		t_size playlist = pm->find_or_create_playlist(playlist_name.get_ptr(), pfc::infinite_size);
		pm->set_active_playlist(playlist);
		pm->playlist_remove_items(playlist, bit_array_true());
		pm->playlist_add_items(playlist, tracks, bit_array_true());
	}
};

void save_xspf_playlist(pfc::string8 xspf_filepath)
{
	pfc::list_t<metadb_handle_ptr> tracks;
	static_api_ptr_t<playlist_manager> pm;
	pm->activeplaylist_get_all_items(tracks);
	pfc::string8 xmlfile;
	xmlfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	xmlfile << "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n";
	xmlfile << "\t<trackList>\n";
	{
		in_metadb_sync sync;

		for (t_size i = 0; i < tracks.get_count(); i++) {
			const file_info* fi;

			if (tracks[i]->get_info_locked(fi)) {
				xmlfile << "\t\t<track>\n";

				if (fi->meta_exists("title")) {
					xmlfile << "\t\t\t<title>" << stringcvt_escape_xml(pfc::stringcvt::string_utf8_from_ansi(fi->meta_get("title", 0))).get_ptr() << "</title>\n";
				}

				if (fi->meta_exists("artist")) {
					xmlfile << "\t\t\t<creator>" << stringcvt_escape_xml(pfc::stringcvt::string_utf8_from_ansi(fi->meta_get("artist", 0))).get_ptr() << "</creator>\n";
				}

				if (fi->meta_exists("album")) {
					xmlfile << "\t\t\t<album>" << stringcvt_escape_xml(pfc::stringcvt::string_utf8_from_ansi(fi->meta_get("album", 0))).get_ptr() << "</album>\n";
				}

				if (fi->meta_exists("tracknumber")) {
					xmlfile << "\t\t\t<trackNum>" << (t_uint32)atoi(fi->meta_get("tracknumber", 0)) << "</trackNum>\n";
				}

				xmlfile << "\t\t\t<duration>" << (t_uint32)(tracks[i]->get_length() * 1000) << "</duration>\n";
				xmlfile << "\t\t</track>\n";
			}
		}
	}
	xmlfile << "\t</trackList>\n";
	xmlfile << "</playlist>\n";
	abort_callback_impl p_abort;
	text_file_loader::write(xspf_filepath, p_abort, xmlfile.get_ptr(), 0);
}




// mainmenu_group

static const GUID guid_mainmenu_group_spl_lastfm = { 0x1e935325, 0xb96a, 0x4cb2, { 0x96, 0x44, 0x1f, 0xea, 0xeb, 0x32, 0x9d, 0xd4 } };
class mainmenu_group_spl_lastfm : public mainmenu_group {
	GUID get_guid() {
		return(guid_mainmenu_group_spl_lastfm);
	}
	GUID get_parent() {
		return(mainmenu_groups::library);
	}
	t_uint32 get_sort_priority() {
		return(0);
	}
};
static mainmenu_commands_factory_t<mainmenu_group_spl_lastfm> foo_mainmenu_group_spl_lastfm;

static const GUID guid_mainmenu_group_spl_xspf = { 0xb8bed196, 0x11bd, 0x4b6e, { 0x88, 0x53, 0x74, 0x28, 0xde, 0x90, 0xd8, 0x29 } };
class mainmenu_group_spl_xspf : public mainmenu_group {
	GUID get_guid() {
		return(guid_mainmenu_group_spl_xspf);
	}
	GUID get_parent() {
		return(mainmenu_groups::library);
	}
	t_uint32 get_sort_priority() {
		return(1);
	}
};
static mainmenu_commands_factory_t<mainmenu_group_spl_xspf> foo_mainmenu_group_spl_xspf;




// mainmenu_group_popup

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp = { 0x4bbbd41, 0x2d63, 0x4ce8, { 0x92, 0x6f, 0x9c, 0xeb, 0xbb, 0xfa, 0xc8, 0xaf } };
class mainmenu_group_popup_spl_lastfm_ttp : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_spl_lastfm);
	}
	t_uint32 get_sort_priority() {
		return(0);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Last.fm Top Tracks";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp> foo_mainmenu_group_popup_spl_lastfm_ttp;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ltp = { 0x9a2381da, 0x5c66, 0x49fa, { 0x89, 0xbf, 0xde, 0x35, 0xd6, 0xb7, 0x96, 0xc0 } };
class mainmenu_group_popup_spl_lastfm_ltp : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ltp);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_spl_lastfm);
	}
	t_uint32 get_sort_priority() {
		return(1);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Last.fm Loved Tracks";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ltp> foo_mainmenu_group_popup_spl_lastfm_ltp;

static const GUID guid_mainmenu_group_popup_spl_lastfm_rtp = { 0xdb82f12a, 0x27bb, 0x48ce, { 0xb4, 0xa, 0x58, 0xab, 0xc5, 0xf9, 0x5, 0xef } };
class mainmenu_group_popup_spl_lastfm_rtp : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_rtp);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_spl_lastfm);
	}
	t_uint32 get_sort_priority() {
		return(2);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Last.fm Recent Tracks";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_rtp> foo_mainmenu_group_popup_spl_lastfm_rtp;

static const GUID guid_mainmenu_group_popup_spl_lastfm_p = { 0xd7ed290b, 0x5f2a, 0x4e81, { 0xa4, 0x9e, 0x1a, 0xa4, 0x72, 0xe3, 0x2f, 0x7b } };
class mainmenu_group_popup_spl_lastfm_p : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_p);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_spl_lastfm);
	}
	t_uint32 get_sort_priority() {
		return(3);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Last.fm Playlists";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_p> foo_mainmenu_group_popup_spl_lastfm_p;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_u = { 0x9298e62b, 0x53a5, 0x496d, { 0xbc, 0x5a, 0xd4, 0x3b, 0x81, 0x6f, 0x21, 0xd9 } };
class mainmenu_group_popup_spl_lastfm_ttp_u : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_u);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(0);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = spl_lastfm_user;
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_u> foo_mainmenu_group_popup_spl_lastfm_ttp_u;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_n = { 0x5dc56f60, 0xbb90, 0x4732, { 0x9d, 0x91, 0x26, 0xdb, 0xa8, 0x5a, 0xb5, 0x62 } };
class mainmenu_group_popup_spl_lastfm_ttp_n : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_n);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(1);
	}
	void get_display_string(pfc::string_base& p_out) {
		pfc::string8 p_out_string;
		p_out_string << spl_lastfm_user << "'s Neighbours";
		p_out = p_out_string;
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_n> foo_mainmenu_group_popup_spl_lastfm_ttp_n;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_f = { 0x5a40a26b, 0xa9a8, 0x4823, { 0xa6, 0x21, 0x44, 0xc4, 0x5e, 0xe, 0x96, 0xa5 } };
class mainmenu_group_popup_spl_lastfm_ttp_f : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_f);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(2);
	}
	void get_display_string(pfc::string_base& p_out) {
		pfc::string8 p_out_string;
		p_out_string << spl_lastfm_user << "'s Friends";
		p_out = p_out_string;
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_f> foo_mainmenu_group_popup_spl_lastfm_ttp_f;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user = { 0x6d93c0a9, 0x1fa9, 0x4d1b, { 0x9d, 0x9f, 0x1f, 0x17, 0x11, 0x92, 0x81, 0xaf } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_user : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(3);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Custom User";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_user> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_user;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_users = { 0x65a9990b, 0xc804, 0x4f0e, { 0x86, 0xcb, 0xd5, 0xeb, 0xfd, 0x69, 0x23, 0x49 } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_users : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_users);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(4);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Custom Users";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_users> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_users;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_top_tags = { 0x7ae230, 0x7a3f, 0x4e09, { 0xbb, 0x27, 0x2, 0xa2, 0x7b, 0xa8, 0x95, 0xc4 } };
class mainmenu_group_popup_spl_lastfm_ttp_top_tags : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_top_tags);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(5);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Top Tags";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_top_tags> foo_mainmenu_group_popup_spl_lastfm_ttp_top_tags;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_tags = { 0x8a5df9c0, 0xfb5d, 0x4ea3, { 0xa5, 0x22, 0x2d, 0x3f, 0x51, 0xa5, 0xd2, 0x21 } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_tags : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_tags);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp);
	}
	t_uint32 get_sort_priority() {
		return(6);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Custom Tags";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_tags> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_tags;




// mainmenu_commands lastfm

class mainmenu_commands_spl_lastfm_ttp_u : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(5);
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_u_overall = { 0xbef25348, 0x2d8d, 0x4fa2, { 0xb9, 0xe4, 0xf6, 0x6c, 0xaa, 0x82, 0xa5, 0x5a } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_u_year = { 0xc663991c, 0xf33d, 0x45e1, { 0xa5, 0xb, 0x67, 0x42, 0x88, 0xf3, 0x31, 0x2 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_u_6month = { 0x2ee037a2, 0x4a07, 0x4829, { 0xa8, 0xc4, 0x52, 0x4a, 0xe8, 0xf9, 0x23, 0x33 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_u_3month = { 0x6f3c5327, 0x900d, 0x4fb9, { 0xb1, 0xb3, 0x88, 0x48, 0xb1, 0x2e, 0x27, 0x30 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_u_week = { 0x96f90695, 0xa739, 0x4ff0, { 0xa3, 0xcd, 0x32, 0xd8, 0x89, 0x6e, 0xf7, 0xea } };

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_u_overall);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_u_year);
		}

		if (p_index == 2) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_u_6month);
		}

		if (p_index == 3) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_u_3month);
		}

		if (p_index == 4) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_u_week);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Overall";
		}

		if (p_index == 1) {
			p_out = "Over Last Year";
		}

		if (p_index == 2) {
			p_out = "Over Last 6 Months";
		}

		if (p_index == 3) {
			p_out = "Over Last 3 Months";
		}

		if (p_index == 4) {
			p_out = "Over Last Week";
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_lastfm_user << "' over the last year.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_lastfm_user << "' over the last 6 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_lastfm_user << "' over the last 3 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 4) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_lastfm_user << "' over the last week.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_u);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_lastfm_user, "overall");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_lastfm_user, "12month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_lastfm_user, "6month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_lastfm_user, "3month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 4 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_lastfm_user, "7day");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_u> foo_mainmenu_commands_spl_lastfm_ttp_u;

class mainmenu_commands_spl_lastfm_ttp_n : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(5);
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_n_overall = { 0xfc55e766, 0xd884, 0x43c6, { 0xb0, 0x3c, 0x14, 0xf4, 0xba, 0xa, 0xc3, 0xa6 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_n_year = { 0x9da6c35a, 0x4fd2, 0x4fb9, { 0x82, 0x82, 0x5f, 0x57, 0x46, 0x27, 0xed, 0x27 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_n_6month = { 0xc7ffa2ae, 0x1d24, 0x491c, { 0x8e, 0x17, 0xdf, 0xa5, 0xd7, 0xec, 0x32, 0xff } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_n_3month = { 0xdbb5e15f, 0x6c6f, 0x400e, { 0x95, 0xe7, 0x59, 0xc4, 0xaa, 0xcd, 0x24, 0x28 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_n_week = { 0xf1faa036, 0xdc8e, 0x46eb, { 0x96, 0xc, 0x8, 0x49, 0xff, 0x1, 0xe6, 0xcf } };

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_n_overall);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_n_year);
		}

		if (p_index == 2) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_n_6month);
		}

		if (p_index == 3) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_n_3month);
		}

		if (p_index == 4) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_n_week);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Overall";
		}

		if (p_index == 1) {
			p_out = "Over Last Year";
		}

		if (p_index == 2) {
			p_out = "Over Last 6 Months";
		}

		if (p_index == 3) {
			p_out = "Over Last 3 Months";
		}

		if (p_index == 4) {
			p_out = "Over Last Week";
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "' over the last year.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "' over the last 6 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "' over the last 3 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 4) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "' over the last week.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_n);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursTopTracksPlaylist>(spl_lastfm_user, "overall");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursTopTracksPlaylist>(spl_lastfm_user, "12month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursTopTracksPlaylist>(spl_lastfm_user, "6month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursTopTracksPlaylist>(spl_lastfm_user, "3month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 4 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursTopTracksPlaylist>(spl_lastfm_user, "7day");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_n> foo_mainmenu_commands_spl_lastfm_ttp_n;

class mainmenu_commands_spl_lastfm_ttp_f : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(5);
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_f_overall = { 0x3582d806, 0xc27a, 0x4e16, { 0xaf, 0x66, 0x5, 0xa, 0x44, 0x8, 0x11, 0x7a } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_f_year = { 0xb31d64df, 0xf8bc, 0x4620, { 0x96, 0x16, 0x4a, 0x7a, 0xa9, 0x35, 0xd7, 0x31 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_f_6month = { 0xefdbf6f1, 0x1eb8, 0x4731, { 0xad, 0x6e, 0x96, 0xab, 0x36, 0xb6, 0xec, 0x28 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_f_3month = { 0x8b3304ac, 0x6fbb, 0x40d1, { 0x9b, 0x2a, 0x84, 0x81, 0x2b, 0xd, 0x68, 0x5f } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_f_week = { 0xb6f061c, 0x311d, 0x4e0a, { 0xb2, 0x5, 0xdc, 0xc1, 0x77, 0xec, 0x4c, 0xdb } };

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_f_overall);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_f_year);
		}

		if (p_index == 2) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_f_6month);
		}

		if (p_index == 3) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_f_3month);
		}

		if (p_index == 4) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_f_week);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Overall";
		}

		if (p_index == 1) {
			p_out = "Over Last Year";
		}

		if (p_index == 2) {
			p_out = "Over Last 6 Months";
		}

		if (p_index == 3) {
			p_out = "Over Last 3 Months";
		}

		if (p_index == 4) {
			p_out = "Over Last Week";
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the friends of Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the friends of Last.fm user '" << spl_lastfm_user << "' over the last year.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the friends of Last.fm user '" << spl_lastfm_user << "' over the last 6 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the friends of Last.fm user '" << spl_lastfm_user << "' over the last 3 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 4) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the friends of Last.fm user '" << spl_lastfm_user << "' over the last week.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_f);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsTopTracksPlaylist>(spl_lastfm_user, "overall");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsTopTracksPlaylist>(spl_lastfm_user, "12month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsTopTracksPlaylist>(spl_lastfm_user, "6month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsTopTracksPlaylist>(spl_lastfm_user, "3month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 4 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsTopTracksPlaylist>(spl_lastfm_user, "7day");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_f> foo_mainmenu_commands_spl_lastfm_ttp_f;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_overall = { 0xd6ca4a2a, 0x1c87, 0x430e, { 0xb2, 0x3c, 0xa5, 0x90, 0x90, 0x94, 0x55, 0x8c } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_user_overall : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_overall);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user);
	}
	t_uint32 get_sort_priority() {
		return(0);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Overall";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_user_overall> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_user_overall;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_year = { 0x5f8100fd, 0x79a0, 0x4664, { 0x83, 0x9, 0x41, 0xe1, 0x8e, 0xdf, 0x3, 0x81 } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_user_year : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_year);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user);
	}
	t_uint32 get_sort_priority() {
		return(1);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Over Last Year";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_user_year> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_user_year;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_6month = { 0x1dba5668, 0x8caf, 0x49bc, { 0x8e, 0xf, 0xea, 0x13, 0x5e, 0x31, 0xe0, 0xc8 } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_user_6month : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_6month);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user);
	}
	t_uint32 get_sort_priority() {
		return(2);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Over Last 6 Months";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_user_6month> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_user_6month;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_3month = { 0x94ff6a86, 0x30fe, 0x4da2, { 0x9e, 0xe3, 0x46, 0x9d, 0xa1, 0xce, 0xde, 0x3 } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_user_3month : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_3month);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user);
	}
	t_uint32 get_sort_priority() {
		return(3);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Over Last 3 Months";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_user_3month> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_user_3month;

static const GUID guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_week = { 0xcc828de8, 0x928f, 0x4eed, { 0xb0, 0x81, 0xbf, 0xa7, 0x3f, 0xae, 0x5c, 0x1e } };
class mainmenu_group_popup_spl_lastfm_ttp_custom_user_week : public mainmenu_group_popup {
	GUID get_guid() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_week);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user);
	}
	t_uint32 get_sort_priority() {
		return(4);
	}
	void get_display_string(pfc::string_base& p_out) {
		p_out = "Over Last Week";
	}
};
static mainmenu_commands_factory_t<mainmenu_group_popup_spl_lastfm_ttp_custom_user_week> foo_mainmenu_group_popup_spl_lastfm_ttp_custom_user_week;

class mainmenu_commands_spl_lastfm_ttp_custom_user_overall : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_custom_user_overall;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_custom_user_overall_base = { 0x00000000, 0x21bb, 0x4c5c, { 0xbf, 0x37, 0xa2, 0xa5, 0x1, 0x6, 0xed, 0x3a } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_overall_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_overall.add_item(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_overall_base);
		}

		if (p_index < spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_overall[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_global_custom_users[p_index] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_overall);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_global_custom_users[p_index], "overall");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_user_overall> foo_mainmenu_commands_spl_lastfm_ttp_custom_user_overall;

class mainmenu_commands_spl_lastfm_ttp_custom_user_year : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_custom_user_year;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_custom_user_year_base = { 0x00000000, 0x7d3b, 0x4eda, { 0x88, 0xdd, 0xad, 0x27, 0xd3, 0xb, 0x9, 0xa6 } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_year_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_year.add_item(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_year_base);
		}

		if (p_index < spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_year[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_global_custom_users[p_index] << "' over the last year.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_year);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_global_custom_users[p_index], "12month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_user_year> foo_mainmenu_commands_spl_lastfm_ttp_custom_user_year;

class mainmenu_commands_spl_lastfm_ttp_custom_user_6month : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_custom_user_6month;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_custom_user_6month_base = { 0x00000000, 0xc836, 0x4ef0, { 0xa0, 0x8, 0x35, 0xad, 0x79, 0x1a, 0x82, 0x32 } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_6month_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_6month.add_item(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_6month_base);
		}

		if (p_index < spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_6month[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_global_custom_users[p_index] << "' over the last 6 months.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_6month);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_global_custom_users[p_index], "6month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_user_6month> foo_mainmenu_commands_spl_lastfm_ttp_custom_user_6month;

class mainmenu_commands_spl_lastfm_ttp_custom_user_3month : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_custom_user_3month;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_custom_user_3month_base = { 0x00000000, 0x851d, 0x48e6, { 0x8f, 0x72, 0xc0, 0x3a, 0xe1, 0x8b, 0xf6, 0xfd } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_3month_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_3month.add_item(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_3month_base);
		}

		if (p_index < spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_3month[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_global_custom_users[p_index] << "' over the last 3 months.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_3month);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_global_custom_users[p_index], "3month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_user_3month> foo_mainmenu_commands_spl_lastfm_ttp_custom_user_3month;

class mainmenu_commands_spl_lastfm_ttp_custom_user_week : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_custom_user_week;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_custom_user_week_base = { 0x00000000, 0xb739, 0x46b7, { 0x8c, 0x11, 0x5f, 0x13, 0x9b, 0xa5, 0xa0, 0xff } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_week_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_custom_user_week.add_item(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_week_base);
		}

		if (p_index < spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_custom_user_week[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for Last.fm user '" << spl_global_custom_users[p_index] << "' over the last week.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_user_week);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserTopTracksPlaylist>(spl_global_custom_users[p_index], "7day");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_user_week> foo_mainmenu_commands_spl_lastfm_ttp_custom_user_week;

class mainmenu_commands_spl_lastfm_ttp_custom_users : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(5);
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_custom_users_overall = { 0xf049dd3a, 0xbd6f, 0x4851, { 0xa3, 0x2c, 0x66, 0x9a, 0x61, 0x77, 0xb0, 0x18 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_custom_users_year = { 0xa30204f7, 0xd211, 0x47c1, { 0x9f, 0x7f, 0xc2, 0xee, 0x85, 0xde, 0x9, 0xee } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_custom_users_6month = { 0x164a0303, 0x7438, 0x414d, { 0xb0, 0xfc, 0x6, 0xd7, 0x3c, 0x74, 0xa8, 0x66 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_custom_users_3month = { 0xfd16323b, 0x9ec0, 0x4513, { 0x9c, 0xbd, 0x7a, 0x8a, 0x78, 0x45, 0xfc, 0xa1 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ttp_custom_users_week = { 0xe42f5e31, 0x51d0, 0x4d89, { 0x9a, 0xb5, 0xc3, 0x83, 0x8d, 0xfd, 0xfc, 0xef } };

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_custom_users_overall);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_custom_users_year);
		}

		if (p_index == 2) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_custom_users_6month);
		}

		if (p_index == 3) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_custom_users_3month);
		}

		if (p_index == 4) {
			return(guid_mainmenu_commands_spl_lastfm_ttp_custom_users_week);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Overall";
		}

		if (p_index == 1) {
			p_out = "Over Last Year";
		}

		if (p_index == 2) {
			p_out = "Over Last 6 Months";
		}

		if (p_index == 3) {
			p_out = "Over Last 3 Months";
		}

		if (p_index == 4) {
			p_out = "Over Last Week";
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the custom Last.fm users.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the custom Last.fm users over the last year.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the custom Last.fm users over the last 6 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the custom Last.fm users over the last 3 months.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 4) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks for the custom Last.fm users over the last week.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_users);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersTopTracksPlaylist>(spl_global_custom_users, "overall");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersTopTracksPlaylist>(spl_global_custom_users, "12month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersTopTracksPlaylist>(spl_global_custom_users, "6month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersTopTracksPlaylist>(spl_global_custom_users, "3month");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 4 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersTopTracksPlaylist>(spl_global_custom_users, "7day");
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_users> foo_mainmenu_commands_spl_lastfm_ttp_custom_users;

class mainmenu_commands_spl_lastfm_ttp_top_tags : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_top_tags.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_top_tags;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_top_tags_base = { 0x00000000, 0x59ed, 0x47a2, { 0xaa, 0xf1, 0xc9, 0x52, 0xdd, 0x48, 0x57, 0x96 } };

		for (t_size i = 0; i < spl_global_top_tags.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_top_tags_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_top_tags.add_item(guids_mainmenu_commands_spl_lastfm_ttp_top_tags_base);
		}

		if (p_index < spl_global_top_tags.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_top_tags[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_top_tags.get_count()) {
			p_out = spl_global_top_tags[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_top_tags.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks on Last.fm with tag '" << spl_global_top_tags[p_index] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_top_tags);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_top_tags.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateTagTopTracksPlaylist>(spl_global_top_tags[p_index]);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_top_tags> foo_mainmenu_commands_spl_lastfm_ttp_top_tags;

class mainmenu_commands_spl_lastfm_ttp_custom_tags : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_custom_tags.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ttp_custom_tags;
		GUID guids_mainmenu_commands_spl_lastfm_ttp_custom_tags_base = { 0x00000000, 0x49d7, 0x4179, { 0x9d, 0xda, 0xa0, 0x66, 0x9, 0x7c, 0xe, 0xb3 } };

		for (t_size i = 0; i < spl_global_custom_tags.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ttp_custom_tags_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ttp_custom_tags.add_item(guids_mainmenu_commands_spl_lastfm_ttp_custom_tags_base);
		}

		if (p_index < spl_global_custom_tags.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ttp_custom_tags[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_tags.get_count()) {
			p_out = spl_global_custom_tags[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_custom_tags.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the top tracks on Last.fm with tag '" << spl_global_custom_tags[p_index] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ttp_custom_tags);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_custom_tags.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateTagTopTracksPlaylist>(spl_global_custom_tags[p_index]);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ttp_custom_tags> foo_mainmenu_commands_spl_lastfm_ttp_custom_tags;

class mainmenu_commands_spl_lastfm_ltp : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(4 + spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_lastfm_ltp_u = { 0xf4aca101, 0x7c3d, 0x4810, { 0xb8, 0xd3, 0x49, 0x1, 0x24, 0xb6, 0xf4, 0xf4 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ltp_n = { 0x4b9585a7, 0x9de3, 0x4b7f, { 0x87, 0xc3, 0x86, 0x71, 0x55, 0x8e, 0x4, 0xc4 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ltp_f = { 0x32883c78, 0xe23e, 0x4384, { 0x9c, 0x40, 0xc9, 0xd0, 0xdd, 0x2e, 0x47, 0x5f } };
		static const GUID guid_mainmenu_commands_spl_lastfm_ltp_c = { 0xefaedb65, 0xaac4, 0x4f23, { 0xa9, 0xa1, 0xd3, 0xc7, 0xd8, 0x94, 0x20, 0xe7 } };
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_ltp_custom_user;
		GUID guids_mainmenu_commands_spl_lastfm_ltp_custom_user_base = { 0x00000000, 0xd518, 0x474c, { 0xac, 0x7, 0x53, 0x62, 0xbb, 0x21, 0x24, 0x14 } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_ltp_custom_user_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_ltp_custom_user.add_item(guids_mainmenu_commands_spl_lastfm_ltp_custom_user_base);
		}

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_lastfm_ltp_u);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_lastfm_ltp_n);
		}

		if (p_index == 2) {
			return(guid_mainmenu_commands_spl_lastfm_ltp_f);
		}

		if (p_index == 3) {
			return(guid_mainmenu_commands_spl_lastfm_ltp_c);
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_ltp_custom_user[p_index - 4]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = spl_lastfm_user;
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << spl_lastfm_user << "'s Neighbours";
			p_out = p_out_string;
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << spl_lastfm_user << "'s Friends";
			p_out = p_out_string;
		}

		if (p_index == 3) {
			p_out = "Custom Users";
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index - 4];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the loved tracks for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the loved tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the loved tracks for the friends of Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the loved tracks for the custom Last.fm users.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the loved tracks for Last.fm user '" << spl_global_custom_users[p_index - 4] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_ltp);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserLovedTracksPlaylist>(spl_lastfm_user);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursLovedTracksPlaylist>(spl_lastfm_user);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsLovedTracksPlaylist>(spl_lastfm_user);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersLovedTracksPlaylist>(spl_global_custom_users);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserLovedTracksPlaylist>(spl_global_custom_users[p_index - 4]);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_ltp> foo_mainmenu_commands_spl_lastfm_ltp;

class mainmenu_commands_spl_lastfm_rtp : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(4 + spl_global_custom_users.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_lastfm_rtp_u = { 0xb48f4b6c, 0x4885, 0x407b, { 0x98, 0x6, 0xc6, 0xaa, 0x8e, 0x2c, 0x80, 0x86 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_rtp_n = { 0x14801737, 0xade5, 0x47e3, { 0xa0, 0x91, 0x52, 0x14, 0xce, 0x6f, 0xdf, 0x2d } };
		static const GUID guid_mainmenu_commands_spl_lastfm_rtp_f = { 0x3cf8239b, 0x3734, 0x4d2a, { 0x9f, 0x57, 0x72, 0xf0, 0xa8, 0x42, 0x49, 0x16 } };
		static const GUID guid_mainmenu_commands_spl_lastfm_rtp_c = { 0x4103b644, 0xcc68, 0x4740, { 0xbd, 0x71, 0x98, 0x6e, 0x1c, 0x38, 0xe6, 0xb3 } };
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_rtp_custom_user;
		GUID guids_mainmenu_commands_spl_lastfm_rtp_custom_user_base = { 0x00000000, 0x46b3, 0x4a99, { 0x9b, 0x3f, 0x25, 0x64, 0xbd, 0xd5, 0x21, 0x90 } };

		for (t_size i = 0; i < spl_global_custom_users.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_rtp_custom_user_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_rtp_custom_user.add_item(guids_mainmenu_commands_spl_lastfm_rtp_custom_user_base);
		}

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_lastfm_rtp_u);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_lastfm_rtp_n);
		}

		if (p_index == 2) {
			return(guid_mainmenu_commands_spl_lastfm_rtp_f);
		}

		if (p_index == 3) {
			return(guid_mainmenu_commands_spl_lastfm_rtp_c);
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_rtp_custom_user[p_index - 4]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = spl_lastfm_user;
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << spl_lastfm_user << "'s Neighbours";
			p_out = p_out_string;
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << spl_lastfm_user << "'s Friends";
			p_out = p_out_string;
		}

		if (p_index == 3) {
			p_out = "Custom Users";
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count()) {
			p_out = spl_global_custom_users[p_index - 4];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the recent tracks for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 1) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the recent tracks for the neighbours of Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 2) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the recent tracks for the friends of Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the recent tracks for the custom Last.fm users.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Generates a playlist with the recent tracks for Last.fm user '" << spl_global_custom_users[p_index - 4] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_rtp);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserRecentTracksPlaylist>(spl_lastfm_user);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateNeighboursRecentTracksPlaylist>(spl_lastfm_user);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateFriendsRecentTracksPlaylist>(spl_lastfm_user);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateCustomUsersRecentTracksPlaylist>(spl_global_custom_users);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}

		if (p_index >= 4 && p_index < 4 + spl_global_custom_users.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateUserRecentTracksPlaylist>(spl_global_custom_users[p_index - 4]);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_rtp> foo_mainmenu_commands_spl_lastfm_rtp;

class mainmenu_commands_spl_lastfm_p : public mainmenu_commands {
	t_uint32 get_command_count() {
		return((t_uint32)spl_global_playlist_ids.get_count());
	}
	GUID get_command(t_uint32 p_index) {
		pfc::list_t<GUID> guids_mainmenu_commands_spl_lastfm_p;
		GUID guids_mainmenu_commands_spl_lastfm_p_base = { 0x00000000, 0xb49c, 0x4008, { 0x94, 0xfa, 0x3c, 0xfc, 0xf1, 0x26, 0x45, 0x9e } };

		for (t_size i = 0; i < spl_global_playlist_ids.get_count(); i++) {
			guids_mainmenu_commands_spl_lastfm_p_base.Data1++;
			guids_mainmenu_commands_spl_lastfm_p.add_item(guids_mainmenu_commands_spl_lastfm_p_base);
		}

		if (p_index < spl_global_playlist_ids.get_count()) {
			return(guids_mainmenu_commands_spl_lastfm_p[p_index]);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_playlist_ids.get_count()) {
			p_out = spl_global_playlist_names[p_index];
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index < spl_global_playlist_ids.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Loads Last.fm playlist '" << spl_global_playlist_names[p_index] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_popup_spl_lastfm_p);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index < spl_global_playlist_ids.get_count() && core_api::assert_main_thread()) {
			service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateTracksPlaylist>(spl_global_playlist_ids[p_index], spl_global_playlist_names[p_index]);
			threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_lastfm_p> foo_mainmenu_commands_spl_lastfm_p;




// mainmenu_commands xspf

class mainmenu_commands_spl_xspf : public mainmenu_commands {
	t_uint32 get_command_count() {
		return(2);
	}
	GUID get_command(t_uint32 p_index) {
		static const GUID guid_mainmenu_commands_spl_xspf_load = { 0x8e8125f3, 0xb90a, 0x4b8c, { 0xb0, 0x66, 0x7c, 0xca, 0xe6, 0xba, 0x38, 0x63 } };
		static const GUID guid_mainmenu_commands_spl_xspf_save = { 0x61ca12dd, 0xb84, 0x4d41, { 0xab, 0x51, 0x36, 0x72, 0x66, 0xc7, 0x7e, 0x3d } };

		if (p_index == 0) {
			return(guid_mainmenu_commands_spl_xspf_load);
		}

		if (p_index == 1) {
			return(guid_mainmenu_commands_spl_xspf_save);
		}

		return(pfc::guid_null);
	}
	void get_name(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Load XSPF Playlist ...";
		}

		if (p_index == 1) {
			p_out = "Save To XSPF Playlist ...";
		}
	}
	bool get_description(t_uint32 p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Loads an XSPF (XML Shareable Playlist Format) playlist.";
			return(true);
		}

		if (p_index == 1) {
			p_out = "Saves the active playlist to an XSPF (XML Shareable Playlist Format) playlist.";
			return(true);
		}

		return(false);
	}
	GUID get_parent() {
		return(guid_mainmenu_group_spl_xspf);
	}
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			pfc::string8 xspf_filepath;

			if (uGetOpenFileName(core_api::get_main_window(), "XSPF Playlists (*.xspf)|*.xspf|All Files (*.*)|*.*", 0, "xspf", "Load XSPF Playlist ...", 0, xspf_filepath, 0)) {
				service_ptr_t<threaded_process_callback> tpc = new service_impl_t<LoadXSPFPlaylist>(xspf_filepath);
				threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
			}
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			pfc::string8 xspf_filepath;

			if (uGetOpenFileName(core_api::get_main_window(), "XSPF Playlists (*.xspf)|*.xspf|All Files (*.*)|*.*", 0, "xspf", "Save To XSPF Playlist ...", 0, xspf_filepath, 1)) {
				save_xspf_playlist(xspf_filepath);
			}
		}
	}
};
static mainmenu_commands_factory_t<mainmenu_commands_spl_xspf> foo_mainmenu_commands_spl_xspf;




// contextmenu_item_simple

static const GUID guid_contextmenu_group_popup_spl_lastfm = { 0xa5dffcaa, 0x4904, 0x4df3, { 0x8f, 0xd5, 0x62, 0xd7, 0x9f, 0x0, 0x97, 0xd4 } };
static contextmenu_group_popup_factory foo_contextmenu_group_popup_spl_lastfm(guid_contextmenu_group_popup_spl_lastfm, contextmenu_groups::root, "Last.fm", 0);

class contextmenu_item_simple_spl : public contextmenu_item_simple {
	GUID get_parent() {
		return(guid_contextmenu_group_popup_spl_lastfm);
	}
	unsigned get_num_items() {
		return(7 + spl_global_playlist_ids.get_count());
	}
	void get_item_name(unsigned p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Last.fm Artist Top Tracks";
		}

		if (p_index == 1) {
			p_out = "Last.fm Similar Artists Top Tracks";
		}

		if (p_index == 2) {
			p_out = "Last.fm Similar Tracks";
		}

		if (p_index == 3) {
			p_out = "Last.fm Love Track";
		}

		if (p_index == 4) {
			p_out = "Last.fm Unlove Track";
		}

		if (p_index == 5) {
			p_out = "Last.fm Ban Track";
		}

		if (p_index == 6) {
			p_out = "Last.fm Unban Track";
		}

		if (p_index >= 7 && p_index < 7 + spl_global_playlist_ids.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Last.fm Add Track To Playlist " << p_index - 6;
			p_out = p_out_string;
		}
	}
	void context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_artist;

				if (get_artist(p_data, track_artist)) {
					service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateArtistTopTracksPlaylist>(track_artist);
					threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
				} else {
					console::printf("foo_softplaylists: failed to get artist top tracks: artist tag not present");
				}
			}
		}

		if (p_index == 1 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_artist;

				if (get_artist(p_data, track_artist)) {
					service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateSimilarArtistsTopTracksPlaylist>(track_artist);
					threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
				} else {
					console::printf("foo_softplaylists: failed to get similar artists top tracks: artist tag not present");
				}
			}
		}

		if (p_index == 2 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_title;
				pfc::string8 track_artist;

				if (get_track_title_artist(p_data, track_title, track_artist)) {
					service_ptr_t<threaded_process_callback> tpc = new service_impl_t<GenerateSimilarTracksPlaylist>(track_title, track_artist);
					threaded_process::g_run_modeless(tpc, threaded_process::flag_show_abort | threaded_process::flag_show_item, core_api::get_main_window(), "Generating Playlist ...", pfc::infinite_size);
				} else {
					console::printf("foo_softplaylists: failed to get similar tracks: title and/or artist tag not present");
				}
			}
		}

		if (p_index == 3 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_title;
				pfc::string8 track_artist;

				if (get_track_title_artist(p_data, track_title, track_artist)) {
					foobar2000_io::abort_callback_dummy p_abort;

					if (api_method_track_title_artist("love", track_title, track_artist, p_abort)) {
						console::printf("foo_softplaylists: successfully loved track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					} else {
						console::printf("foo_softplaylists: failed to love track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					}
				} else {
					console::printf("foo_softplaylists: failed to love track: title and/or artist tag not present");
				}
			}
		}

		if (p_index == 4 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_title;
				pfc::string8 track_artist;

				if (get_track_title_artist(p_data, track_title, track_artist)) {
					foobar2000_io::abort_callback_dummy p_abort;

					if (api_method_track_title_artist("unlove", track_title, track_artist, p_abort)) {
						console::printf("foo_softplaylists: successfully unloved track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					} else {
						console::printf("foo_softplaylists: failed to unlove track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					}
				} else {
					console::printf("foo_softplaylists: failed to unlove track: title and/or artist tag not present");
				}
			}
		}

		if (p_index == 5 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_title;
				pfc::string8 track_artist;

				if (get_track_title_artist(p_data, track_title, track_artist)) {
					foobar2000_io::abort_callback_dummy p_abort;

					if (api_method_track_title_artist("ban", track_title, track_artist, p_abort)) {
						console::printf("foo_softplaylists: successfully banned track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					} else {
						console::printf("foo_softplaylists: failed to ban track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					}
				} else {
					console::printf("foo_softplaylists: failed to ban track: title and/or artist tag not present");
				}
			}
		}

		if (p_index == 6 && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_title;
				pfc::string8 track_artist;

				if (get_track_title_artist(p_data, track_title, track_artist)) {
					foobar2000_io::abort_callback_dummy p_abort;

					if (api_method_track_title_artist("unban", track_title, track_artist, p_abort)) {
						console::printf("foo_softplaylists: successfully unbanned track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					} else {
						console::printf("foo_softplaylists: failed to unban track '%s' by '%s'", track_title.get_ptr(), track_artist.get_ptr());
					}
				} else {
					console::printf("foo_softplaylists: failed to unban track: title and/or artist tag not present");
				}
			}
		}

		if (p_index >= 7 && p_index < 7 + spl_global_playlist_ids.get_count() && core_api::assert_main_thread()) {
			if (p_data.get_count() > 0) {
				pfc::string8 track_title;
				pfc::string8 track_artist;

				if (get_track_title_artist(p_data, track_title, track_artist)) {
					foobar2000_io::abort_callback_dummy p_abort;

					if (api_method_playlist_track_title_artist("addtrack", spl_global_playlist_ids[p_index - 7], track_title, track_artist, p_abort)) {
						console::printf("foo_softplaylists: successfully added track '%s' by '%s' to playlist '%s'", track_title.get_ptr(), track_artist.get_ptr(), spl_global_playlist_names[p_index - 7].get_ptr());
					} else {
						console::printf("foo_softplaylists: failed to add track '%s' by '%s' to playlist '%s'", track_title.get_ptr(), track_artist.get_ptr(), spl_global_playlist_names[p_index - 7].get_ptr());
					}
				} else {
					console::printf("foo_softplaylists: failed to add track: title and/or artist tag not present");
				}
			}
		}
	}
	bool context_get_display(unsigned p_index, metadb_handle_list_cref p_data, pfc::string_base& p_out, unsigned& p_displayflags, const GUID& p_caller) {
		if (p_index == 0) {
			pfc::string8 track_artist;

			if (get_artist(p_data, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Artist '" << track_artist << "' Top Tracks";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index == 1) {
			pfc::string8 track_artist;

			if (get_artist(p_data, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Artist '" << track_artist << "' Similar Artists Top Tracks";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index == 2) {
			pfc::string8 track_title;
			pfc::string8 track_artist;

			if (get_track_title_artist(p_data, track_title, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Track '" << track_title << "' By '" << track_artist << "' Level " << spl_similar_tracks_recursion_limit << " Similar Tracks";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index == 3) {
			pfc::string8 track_title;
			pfc::string8 track_artist;

			if (get_track_title_artist(p_data, track_title, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Love Track '" << track_title << "' By '" << track_artist << "'";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index == 4) {
			pfc::string8 track_title;
			pfc::string8 track_artist;

			if (get_track_title_artist(p_data, track_title, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Unlove Track '" << track_title << "' By '" << track_artist << "'";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index == 5) {
			pfc::string8 track_title;
			pfc::string8 track_artist;

			if (get_track_title_artist(p_data, track_title, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Ban Track '" << track_title << "' By '" << track_artist << "'";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index == 6) {
			pfc::string8 track_title;
			pfc::string8 track_artist;

			if (get_track_title_artist(p_data, track_title, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Unban Track '" << track_title << "' By '" << track_artist << "'";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		if (p_index >= 7 && p_index < 7 + spl_global_playlist_ids.get_count()) {
			pfc::string8 track_title;
			pfc::string8 track_artist;

			if (get_track_title_artist(p_data, track_title, track_artist)) {
				pfc::string8 p_out_string;
				p_out_string << "Last.fm Add Track '" << track_title << "' By '" << track_artist << "' To Playlist '" << spl_global_playlist_names[p_index - 7] << "'";
				p_out = p_out_string;
			} else {
				get_item_name(p_index, p_out);
			}

			return(true);
		}

		return(false);
	}
	GUID get_item_guid(unsigned p_index) {
		static const GUID guid_contextmenu_item_simple_spl_artist_ttp = { 0xf6551356, 0xf794, 0x472f, { 0xa3, 0x74, 0xb9, 0x84, 0xc6, 0xb0, 0x32, 0x5 } };
		static const GUID guid_contextmenu_item_simple_spl_similar_artists_ttp = { 0xe922334d, 0xa3c1, 0x45f2, { 0xae, 0x36, 0xdb, 0x65, 0xcf, 0x23, 0xde, 0x7e } };
		static const GUID guid_contextmenu_item_simple_spl_similar_tracks = { 0x1ac77a99, 0xc2b4, 0x4579, { 0xa3, 0xc9, 0xdb, 0x5c, 0xe, 0x16, 0xdf, 0x77 } };
		static const GUID guid_contextmenu_item_simple_spl_love_track = { 0xc39006dd, 0x54f0, 0x41c0, { 0xb5, 0x62, 0x88, 0xb6, 0x8a, 0x18, 0x74, 0x7c } };
		static const GUID guid_contextmenu_item_simple_spl_unlove_track = { 0x8a210201, 0x9aff, 0x4418, { 0xb3, 0x2b, 0x3b, 0x2c, 0x89, 0x5b, 0x1a, 0x7a } };
		static const GUID guid_contextmenu_item_simple_spl_ban_track = { 0xde700ad4, 0x18ea, 0x428b, { 0x97, 0xb2, 0x7b, 0x8a, 0x35, 0x16, 0x57, 0x10 } };
		static const GUID guid_contextmenu_item_simple_spl_unban_track = { 0xd872ef82, 0x83d1, 0x4a92, { 0xb7, 0x54, 0xb8, 0xc2, 0xc, 0x59, 0x23, 0xc1 } };
		pfc::list_t<GUID> guid_contextmenu_item_simple_spl_add_track;
		GUID guid_contextmenu_item_simple_spl_add_track_base = { 0x00000000, 0x246, 0x48a9, { 0x95, 0x33, 0x36, 0xf2, 0x6d, 0x9e, 0xdc, 0xc4 } };

		for (t_size i = 0; i < spl_global_playlist_ids.get_count(); i++) {
			guid_contextmenu_item_simple_spl_add_track_base.Data1++;
			guid_contextmenu_item_simple_spl_add_track.add_item(guid_contextmenu_item_simple_spl_add_track_base);
		}

		if (p_index == 0) {
			return(guid_contextmenu_item_simple_spl_artist_ttp);
		}

		if (p_index == 1) {
			return(guid_contextmenu_item_simple_spl_similar_artists_ttp);
		}

		if (p_index == 2) {
			return(guid_contextmenu_item_simple_spl_similar_tracks);
		}

		if (p_index == 3) {
			return(guid_contextmenu_item_simple_spl_love_track);
		}

		if (p_index == 4) {
			return(guid_contextmenu_item_simple_spl_unlove_track);
		}

		if (p_index == 5) {
			return(guid_contextmenu_item_simple_spl_ban_track);
		}

		if (p_index == 6) {
			return(guid_contextmenu_item_simple_spl_unban_track);
		}

		if (p_index >= 7 && p_index < 7 + spl_global_playlist_ids.get_count()) {
			return(guid_contextmenu_item_simple_spl_add_track[p_index - 7]);
		}

		return(pfc::guid_null);
	}
	bool get_item_description(unsigned p_index, pfc::string_base& p_out) {
		if (p_index == 0) {
			p_out = "Generates a playlist with the top tracks on Last.fm for the selected artist.";
			return(true);
		}

		if (p_index == 1) {
			p_out = "Generates a playlist with the top tracks on Last.fm for artists similar to the selected artist.";
			return(true);
		}

		if (p_index == 2) {
			p_out = "Generates a playlist with tracks on Last.fm similar to the selected track.";
			return(true);
		}

		if (p_index == 3) {
			pfc::string8 p_out_string;
			p_out_string << "Love the selected track for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 4) {
			pfc::string8 p_out_string;
			p_out_string << "Unlove the selected track for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 5) {
			pfc::string8 p_out_string;
			p_out_string << "Ban the selected track for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index == 6) {
			pfc::string8 p_out_string;
			p_out_string << "Unban the selected track for Last.fm user '" << spl_lastfm_user << "'.";
			p_out = p_out_string;
			return(true);
		}

		if (p_index >= 7 && p_index < 7 + spl_global_playlist_ids.get_count()) {
			pfc::string8 p_out_string;
			p_out_string << "Add the selected track to Last.fm playlist '" << spl_global_playlist_names[p_index - 7] << "'.";
			p_out = p_out_string;
			return(true);
		}

		return(false);
	}
};
static contextmenu_item_factory_t<contextmenu_item_simple_spl> foo_contextmenu_item_simple_spl;
