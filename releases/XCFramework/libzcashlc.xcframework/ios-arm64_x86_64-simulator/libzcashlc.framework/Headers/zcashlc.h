#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct TorRuntime TorRuntime;

/**
 * A struct that contains details about an account in the wallet.
 */
typedef struct FfiAccount {
  uint8_t seed_fingerprint[32];
  uint32_t account_index;
} FfiAccount;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of [`FfiAccount`] values.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<FfiAccount>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single allocated
 *     object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     [`FfiAccount`].
 * - The total size `len * mem::size_of::<FfiAccount>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of pointer::offset.
 * - See the safety documentation of [`FfiAccount`]
 */
typedef struct FfiAccounts {
  struct FfiAccount *ptr;
  uintptr_t len;
} FfiAccounts;

/**
 * A struct that contains an account identifier along with a pointer to the binary encoding
 * of an associated key.
 *
 * # Safety
 *
 * - `encoding` must be non-null and must point to an array of `encoding_len` bytes.
 */
typedef struct FFIBinaryKey {
  uint32_t account_id;
  uint8_t *encoding;
  uintptr_t encoding_len;
} FFIBinaryKey;

/**
 * A struct that contains an account identifier along with a pointer to the string encoding
 * of an associated key.
 *
 * # Safety
 *
 * - `encoding` must be non-null and must point to a null-terminated UTF-8 string.
 */
typedef struct FFIEncodedKey {
  uint32_t account_id;
  char *encoding;
} FFIEncodedKey;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of [`FFIEncodedKey`] values.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<FFIEncodedKey>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single allocated
 *     object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     [`FFIEncodedKey`].
 * - The total size `len * mem::size_of::<FFIEncodedKey>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of pointer::offset.
 * - See the safety documentation of [`FFIEncodedKey`]
 */
typedef struct FFIEncodedKeys {
  struct FFIEncodedKey *ptr;
  uintptr_t len;
} FFIEncodedKeys;

/**
 * A struct that contains a subtree root.
 *
 * # Safety
 *
 * - `root_hash_ptr` must be non-null and must be valid for reads for `root_hash_ptr_len`
 *   bytes, and it must have an alignment of `1`.
 * - The total size `root_hash_ptr_len` of the slice pointed to by `root_hash_ptr` must
 *   be no larger than `isize::MAX`. See the safety documentation of `pointer::offset`.
 */
typedef struct FfiSubtreeRoot {
  uint8_t *root_hash_ptr;
  uintptr_t root_hash_ptr_len;
  uint32_t completing_block_height;
} FfiSubtreeRoot;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of [`FfiSubtreeRoot`] values.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<FfiSubtreeRoot>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single
 *     allocated object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     [`FfiSubtreeRoot`].
 * - The total size `len * mem::size_of::<FfiSubtreeRoot>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of
 *   `pointer::offset`.
 * - See the safety documentation of [`FfiSubtreeRoot`]
 */
typedef struct FfiSubtreeRoots {
  struct FfiSubtreeRoot *ptr;
  uintptr_t len;
} FfiSubtreeRoots;

/**
 * Balance information for a value within a single pool in an account.
 */
typedef struct FfiBalance {
  /**
   * The value in the account that may currently be spent; it is possible to compute witnesses
   * for all the notes that comprise this value, and all of this value is confirmed to the
   * required confirmation depth.
   */
  int64_t spendable_value;
  /**
   * The value in the account of shielded change notes that do not yet have sufficient
   * confirmations to be spendable.
   */
  int64_t change_pending_confirmation;
  /**
   * The value in the account of all remaining received notes that either do not have sufficient
   * confirmations to be spendable, or for which witnesses cannot yet be constructed without
   * additional scanning.
   */
  int64_t value_pending_spendability;
} FfiBalance;

/**
 * Balance information for a single account.
 *
 * The sum of this struct's fields is the total balance of the account.
 */
typedef struct FfiAccountBalance {
  uint32_t account_id;
  /**
   * The value of unspent Sapling outputs belonging to the account.
   */
  struct FfiBalance sapling_balance;
  /**
   * The value of unspent Orchard outputs belonging to the account.
   */
  struct FfiBalance orchard_balance;
  /**
   * The value of all unspent transparent outputs belonging to the account,
   * irrespective of confirmation depth.
   *
   * Unshielded balances are not subject to confirmation-depth constraints, because the
   * only possible operation on a transparent balance is to shield it, it is possible
   * to create a zero-conf transaction to perform that shielding, and the resulting
   * shielded notes will be subject to normal confirmation rules.
   */
  int64_t unshielded;
} FfiAccountBalance;

/**
 * A struct that contains details about scan progress.
 *
 * When `denominator` is zero, the numerator encodes a non-progress indicator:
 * - 0: progress is unknown.
 * - 1: an error occurred.
 */
typedef struct FfiScanProgress {
  uint64_t numerator;
  uint64_t denominator;
} FfiScanProgress;

/**
 * A type representing the potentially-spendable value of unspent outputs in the wallet.
 *
 * The balances reported using this data structure may overestimate the total spendable
 * value of the wallet, in the case that the spend of a previously received shielded note
 * has not yet been detected by the process of scanning the chain. The balances reported
 * using this data structure can only be certain to be unspent in the case that
 * [`Self::is_synced`] is true, and even in this circumstance it is possible that a newly
 * created transaction could conflict with a not-yet-mined transaction in the mempool.
 *
 * # Safety
 *
 * - `account_balances` must be non-null and must be valid for reads for
 *   `account_balances_len * mem::size_of::<FfiAccountBalance>()` many bytes, and it must
 *   be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `account_balances` must be contained within
 *     a single allocated object. Slices can never span across multiple allocated objects.
 *   - `account_balances` must be non-null and aligned even for zero-length slices.
 *   - `account_balances` must point to `len` consecutive properly initialized values of
 *     type [`FfiAccountBalance`].
 * - The total size `account_balances_len * mem::size_of::<FfiAccountBalance>()` of the
 *   slice pointed to by `account_balances` must be no larger than `isize::MAX`. See the
 *   safety documentation of `pointer::offset`.
 * - `scan_progress` must, if non-null, point to a struct having the layout of
 *   [`FfiScanProgress`].
 */
typedef struct FfiWalletSummary {
  struct FfiAccountBalance *account_balances;
  uintptr_t account_balances_len;
  int32_t chain_tip_height;
  int32_t fully_scanned_height;
  struct FfiScanProgress *scan_progress;
  uint64_t next_sapling_subtree_index;
  uint64_t next_orchard_subtree_index;
} FfiWalletSummary;

/**
 * A struct that contains the start (inclusive) and end (exclusive) of a range of blocks
 * to scan.
 */
typedef struct FfiScanRange {
  int32_t start;
  int32_t end;
  uint8_t priority;
} FfiScanRange;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of [`FfiScanRange`] values.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<FfiScanRange>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single
 *     allocated object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     [`FfiScanRange`].
 * - The total size `len * mem::size_of::<FfiScanRange>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of
 *   `pointer::offset`.
 */
typedef struct FfiScanRanges {
  struct FfiScanRange *ptr;
  uintptr_t len;
} FfiScanRanges;

/**
 * Metadata about modifications to the wallet state made in the course of scanning a set
 * of blocks.
 */
typedef struct FfiScanSummary {
  int32_t scanned_start;
  int32_t scanned_end;
  uint64_t spent_sapling_note_count;
  uint64_t received_sapling_note_count;
} FfiScanSummary;

typedef struct FFIBlockMeta {
  uint32_t height;
  uint8_t *block_hash_ptr;
  uintptr_t block_hash_ptr_len;
  uint32_t block_time;
  uint32_t sapling_outputs_count;
  uint32_t orchard_actions_count;
} FFIBlockMeta;

typedef struct FFIBlocksMeta {
  struct FFIBlockMeta *ptr;
  uintptr_t len;
} FFIBlocksMeta;

/**
 * A struct that optionally contains a pointer to, and length information for, a
 * heap-allocated boxed slice.
 *
 * This is an FFI representation of `Option<Box<[u8]>>`.
 *
 * # Safety
 *
 * - If `ptr` is non-null, it must be valid for reads for `len` bytes, and it must have
 *   an alignment of `1`.
 * - The memory referenced by `ptr` must not be mutated for the lifetime of the struct
 *   (up until [`zcashlc_free_boxed_slice`] is called with it).
 * - The total size `len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 *   - When `ptr` is null, `len` should be zero.
 */
typedef struct FfiBoxedSlice {
  uint8_t *ptr;
  uintptr_t len;
} FfiBoxedSlice;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of `[u8; 32]` arrays.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<[u8; 32]>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single
 *     allocated object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     `[u8; 32]`.
 * - The total size `len * mem::size_of::<[u8; 32]>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of
 *   `pointer::offset`.
 */
typedef struct FfiTxIds {
  uint8_t (*ptr)[32];
  uintptr_t len;
} FfiTxIds;

/**
 * Metadata about the status of a transaction obtained by inspecting the chain state.
 */
enum FfiTransactionStatus_Tag {
  /**
   * The requested transaction ID was not recognized by the node.
   */
  TxidNotRecognized,
  /**
   * The requested transaction ID corresponds to a transaction that is recognized by the node,
   * but is in the mempool or is otherwise not mined in the main chain (but may have been mined
   * on a fork that was reorged away).
   */
  NotInMainChain,
  /**
   * The requested transaction ID corresponds to a transaction that has been included in the
   * block at the provided height.
   */
  Mined,
};
typedef uint8_t FfiTransactionStatus_Tag;

typedef struct FfiTransactionStatus {
  FfiTransactionStatus_Tag tag;
  union {
    struct {
      uint32_t mined;
    };
  };
} FfiTransactionStatus;

/**
 * A request for transaction data enhancement, spentness check, or discovery
 * of spends from a given transparent address within a specific block range.
 */
enum FfiTransactionDataRequest_Tag {
  /**
   * Information about the chain's view of a transaction is requested.
   *
   * The caller evaluating this request on behalf of the wallet backend should respond to this
   * request by determining the status of the specified transaction with respect to the main
   * chain; if using `lightwalletd` for access to chain data, this may be obtained by
   * interpreting the results of the [`GetTransaction`] RPC method. It should then call
   * [`WalletWrite::set_transaction_status`] to provide the resulting transaction status
   * information to the wallet backend.
   *
   * [`GetTransaction`]: crate::proto::service::compact_tx_streamer_client::CompactTxStreamerClient::get_transaction
   */
  GetStatus,
  /**
   * Transaction enhancement (download of complete raw transaction data) is requested.
   *
   * The caller evaluating this request on behalf of the wallet backend should respond to this
   * request by providing complete data for the specified transaction to
   * [`wallet::decrypt_and_store_transaction`]; if using `lightwalletd` for access to chain
   * state, this may be obtained via the [`GetTransaction`] RPC method. If no data is available
   * for the specified transaction, this should be reported to the backend using
   * [`WalletWrite::set_transaction_status`]. A [`TransactionDataRequest::Enhancement`] request
   * subsumes any previously existing [`TransactionDataRequest::GetStatus`] request.
   *
   * [`GetTransaction`]: crate::proto::service::compact_tx_streamer_client::CompactTxStreamerClient::get_transaction
   */
  Enhancement,
  /**
   * Information about transactions that receive or spend funds belonging to the specified
   * transparent address is requested.
   *
   * Fully transparent transactions, and transactions that do not contain either shielded inputs
   * or shielded outputs belonging to the wallet, may not be discovered by the process of chain
   * scanning; as a consequence, the wallet must actively query to find transactions that spend
   * such funds. Ideally we'd be able to query by [`OutPoint`] but this is not currently
   * functionality that is supported by the light wallet server.
   *
   * The caller evaluating this request on behalf of the wallet backend should respond to this
   * request by detecting transactions involving the specified address within the provided block
   * range; if using `lightwalletd` for access to chain data, this may be performed using the
   * [`GetTaddressTxids`] RPC method. It should then call [`wallet::decrypt_and_store_transaction`]
   * for each transaction so detected.
   *
   * [`GetTaddressTxids`]: crate::proto::service::compact_tx_streamer_client::CompactTxStreamerClient::get_taddress_txids
   */
  SpendsFromAddress,
};
typedef uint8_t FfiTransactionDataRequest_Tag;

typedef struct SpendsFromAddress_Body {
  char *address;
  uint32_t block_range_start;
  /**
   * An optional end height; no end height is represented as `-1`
   */
  int64_t block_range_end;
} SpendsFromAddress_Body;

typedef struct FfiTransactionDataRequest {
  FfiTransactionDataRequest_Tag tag;
  union {
    struct {
      uint8_t get_status[32];
    };
    struct {
      uint8_t enhancement[32];
    };
    SpendsFromAddress_Body spends_from_address;
  };
} FfiTransactionDataRequest;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of [`FfiTransactionDataRequest`] values.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<FfiTransactionDataRequest>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single allocated
 *     object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     [`FfiTransactionDataRequest`].
 * - The total size `len * mem::size_of::<FfiTransactionDataRequest>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of pointer::offset.
 * - See the safety documentation of [`FfiTransactionDataRequest`]
 */
typedef struct FfiTransactionDataRequests {
  struct FfiTransactionDataRequest *ptr;
  uintptr_t len;
} FfiTransactionDataRequests;

/**
 * A decimal suitable for converting into an `NSDecimalNumber`.
 */
typedef struct Decimal {
  uint64_t mantissa;
  int16_t exponent;
  bool is_sign_negative;
} Decimal;

/**
 * Initializes global Rust state, such as the logging infrastructure and threadpools.
 *
 * `log_level` defines how the Rust layer logs its events. These values are supported,
 * each level logging more information in addition to the earlier levels:
 * - `off`: The logs are completely disabled.
 * - `error`: Logs very serious errors.
 * - `warn`: Logs hazardous situations.
 * - `info`: Logs useful information.
 * - `debug`: Logs lower priority information.
 * - `trace`: Logs very low priority, often extremely verbose, information.
 *
 * # Safety
 *
 * - The memory pointed to by `log_level` must contain a valid nul terminator at the end
 *   of the string.
 * - `log_level` must be valid for reads of bytes up to and including the nul terminator.
 *   This means in particular:
 *   - The entire memory range of this `CStr` must be contained within a single allocated
 *     object!
 * - The memory referenced by the returned `CStr` must not be mutated for the duration of
 *   the function call.
 * - The nul terminator must be within `isize::MAX` from `log_level`.
 *
 * # Panics
 *
 * This method panics if called more than once.
 */
void zcashlc_init_on_load(const char *log_level);

/**
 * Returns the length of the last error message to be logged.
 */
int32_t zcashlc_last_error_length(void);

/**
 * Copies the last error message into the provided allocated buffer.
 *
 * # Safety
 *
 * - `buf` must be non-null and valid for reads for `length` bytes, and it must have an alignment
 *   of `1`.
 * - The memory referenced by `buf` must not be mutated for the duration of the function call.
 * - The total size `length` must be no larger than `isize::MAX`. See the safety documentation of
 *   pointer::offset.
 */
int32_t zcashlc_error_message_utf8(char *buf, int32_t length);

/**
 * Clears the record of the last error message.
 */
void zcashlc_clear_last_error(void);

/**
 * Sets up the internal structure of the data database.  The value for `seed` may be provided as a
 * null pointer if the caller wishes to attempt migrations without providing the wallet's seed
 * value.
 *
 * Returns:
 * - 0 if successful.
 * - 1 if the seed must be provided in order to execute the requested migrations
 * - 2 if the provided seed is not relevant to any of the derived accounts in the wallet.
 * - -1 on error.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 */
int32_t zcashlc_init_data_database(const uint8_t *db_data,
                                   uintptr_t db_data_len,
                                   const uint8_t *seed,
                                   uintptr_t seed_len,
                                   uint32_t network_id);

/**
 * Frees an array of FfiAccounts values as allocated by `zcashlc_list_accounts`.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FfiAccounts`].
 *   See the safety documentation of [`FfiAccounts`].
 */
void zcashlc_free_accounts(struct FfiAccounts *ptr);

/**
 * Returns a list of the accounts in the wallet.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_free_accounts`] to free the memory associated with the returned pointer
 *   when done using it.
 */
struct FfiAccounts *zcashlc_list_accounts(const uint8_t *db_data,
                                          uintptr_t db_data_len,
                                          uint32_t network_id);

/**
 * Frees a FFIBinaryKey value
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FFIBinaryKey`].
 *   See the safety documentation of [`FFIBinaryKey`].
 */
void zcashlc_free_binary_key(struct FFIBinaryKey *ptr);

/**
 * Adds the next available account-level spend authority, given the current set of [ZIP 316]
 * account identifiers known, to the wallet database.
 *
 * Returns the newly created [ZIP 316] account identifier, along with the binary encoding of the
 * [`UnifiedSpendingKey`] for the newly created account.  The caller should manage the memory of
 * (and store) the returned spending keys in a secure fashion.
 *
 * If `seed` was imported from a backup and this method is being used to restore a
 * previous wallet state, you should use this method to add all of the desired
 * accounts before scanning the chain from the seed's birthday height.
 *
 * By convention, wallets should only allow a new account to be generated after funds
 * have been received by the currently available account (in order to enable
 * automated account recovery).
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 * - Call [`zcashlc_free_binary_key`] to free the memory associated with the returned pointer when
 *   you are finished using it.
 *
 * [ZIP 316]: https://zips.z.cash/zip-0316
 */
struct FFIBinaryKey *zcashlc_create_account(const uint8_t *db_data,
                                            uintptr_t db_data_len,
                                            const uint8_t *seed,
                                            uintptr_t seed_len,
                                            const uint8_t *treestate,
                                            uintptr_t treestate_len,
                                            int64_t recover_until,
                                            uint32_t network_id);

/**
 * Checks whether the given seed is relevant to any of the accounts in the wallet.
 *
 * Returns:
 * - `1` for `Ok(true)`.
 * - `0` for `Ok(false)`.
 * - `-1` for `Err(_)`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 */
int8_t zcashlc_is_seed_relevant_to_any_derived_account(const uint8_t *db_data,
                                                       uintptr_t db_data_len,
                                                       const uint8_t *seed,
                                                       uintptr_t seed_len,
                                                       uint32_t network_id);

/**
 * Frees an array of `FFIEncodedKeys` values as allocated by `zcashlc_list_transparent_receivers`.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FFIEncodedKeys`].
 *   See the safety documentation of [`FFIEncodedKeys`].
 */
void zcashlc_free_keys(struct FFIEncodedKeys *ptr);

/**
 * Returns the most-recently-generated unified payment address for the specified account.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *zcashlc_get_current_address(const uint8_t *db_data,
                                  uintptr_t db_data_len,
                                  int32_t account,
                                  uint32_t network_id);

/**
 * Returns a newly-generated unified payment address for the specified account, with the next
 * available diversifier.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *zcashlc_get_next_available_address(const uint8_t *db_data,
                                         uintptr_t db_data_len,
                                         int32_t account,
                                         uint32_t network_id);

/**
 * Returns a list of the transparent receivers for the diversified unified addresses that have
 * been allocated for the provided account.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_free_keys`] to free the memory associated with the returned pointer
 *   when done using it.
 */
struct FFIEncodedKeys *zcashlc_list_transparent_receivers(const uint8_t *db_data,
                                                          uintptr_t db_data_len,
                                                          int32_t account_id,
                                                          uint32_t network_id);

/**
 * Returns the verified transparent balance for `address`, which ignores utxos that have been
 * received too recently and are not yet deemed spendable according to `min_confirmations`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t zcashlc_get_verified_transparent_balance(const uint8_t *db_data,
                                                 uintptr_t db_data_len,
                                                 const char *address,
                                                 uint32_t network_id,
                                                 uint32_t min_confirmations);

/**
 * Returns the verified transparent balance for `account`, which ignores utxos that have been
 * received too recently and are not yet deemed spendable according to `min_confirmations`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t zcashlc_get_verified_transparent_balance_for_account(const uint8_t *db_data,
                                                             uintptr_t db_data_len,
                                                             uint32_t network_id,
                                                             int32_t account,
                                                             uint32_t min_confirmations);

/**
 * Returns the balance for `address`, including all UTXOs that we know about.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t zcashlc_get_total_transparent_balance(const uint8_t *db_data,
                                              uintptr_t db_data_len,
                                              const char *address,
                                              uint32_t network_id);

/**
 * Returns the balance for `account`, including all UTXOs that we know about.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t zcashlc_get_total_transparent_balance_for_account(const uint8_t *db_data,
                                                          uintptr_t db_data_len,
                                                          uint32_t network_id,
                                                          int32_t account);

/**
 * Returns the memo for a note by copying the corresponding bytes to the received
 * pointer in `memo_bytes_ret`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `txid_bytes` must be non-null and valid for reads for 32 bytes, and it must have an alignment
 *   of `1`.
 * - `memo_bytes_ret` must be non-null and must point to an allocated 512-byte region of memory.
 */
bool zcashlc_get_memo(const uint8_t *db_data,
                      uintptr_t db_data_len,
                      const uint8_t *txid_bytes,
                      uint32_t output_pool,
                      uint16_t output_index,
                      uint8_t *memo_bytes_ret,
                      uint32_t network_id);

/**
 * Returns the memo for a note, if it is known and a valid UTF-8 string.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `txid_bytes` must be non-null and valid for reads for 32 bytes, and it must have an alignment
 *   of `1`.
 * - Call [`zcashlc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *zcashlc_get_memo_as_utf8(const uint8_t *db_data,
                               uintptr_t db_data_len,
                               const uint8_t *txid_bytes,
                               uint16_t output_index,
                               uint32_t network_id);

/**
 * Returns a ZIP-32 signature of the given seed bytes.
 *
 * # Safety
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be at least 32 no larger than `252`. See the safety documentation
 *   of pointer::offset.
 */
bool zcashlc_seed_fingerprint(const uint8_t *seed,
                              uintptr_t seed_len,
                              uint8_t *signature_bytes_ret);

/**
 * Rewinds the data database to at most the given height.
 *
 * If the requested height is greater than or equal to the height of the last scanned block, this
 * function sets the `safe_rewind_ret` output parameter to `-1` and does nothing else.
 *
 * This procedure returns the height to which the database was actually rewound, or `-1` if no
 * rewind was performed.
 *
 * If the requested rewind could not be performed, but a rewind to a different (greater) height
 * would be valid, the `safe_rewind_ret` output parameter will be set to that value on completion;
 * otherwise, it will be set to `-1`.
 *
 * # Safety
 *
 * - `safe_rewind_ret` must be non-null, aligned, and valid for writing an `int64_t`.
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int64_t zcashlc_rewind_to_height(const uint8_t *db_data,
                                 uintptr_t db_data_len,
                                 uint32_t height,
                                 uint32_t network_id,
                                 int64_t *safe_rewind_ret);

/**
 * Adds a sequence of Sapling subtree roots to the data store.
 *
 * Returns true if the subtrees could be stored, false otherwise. When false is returned,
 * caller should check for errors.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `roots` must be non-null and initialized.
 * - The memory referenced by `roots` must not be mutated for the duration of the function call.
 */
bool zcashlc_put_sapling_subtree_roots(const uint8_t *db_data,
                                       uintptr_t db_data_len,
                                       uint64_t start_index,
                                       const struct FfiSubtreeRoots *roots,
                                       uint32_t network_id);

/**
 * Adds a sequence of Orchard subtree roots to the data store.
 *
 * Returns true if the subtrees could be stored, false otherwise. When false is returned,
 * caller should check for errors.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `roots` must be non-null and initialized.
 * - The memory referenced by `roots` must not be mutated for the duration of the function call.
 */
bool zcashlc_put_orchard_subtree_roots(const uint8_t *db_data,
                                       uintptr_t db_data_len,
                                       uint64_t start_index,
                                       const struct FfiSubtreeRoots *roots,
                                       uint32_t network_id);

/**
 * Updates the wallet's view of the blockchain.
 *
 * This method is used to provide the wallet with information about the state of the blockchain,
 * and detect any previously scanned data that needs to be re-validated before proceeding with
 * scanning. It should be called at wallet startup prior to calling `zcashlc_suggest_scan_ranges`
 * in order to provide the wallet with the information it needs to correctly prioritize scanning
 * operations.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 */
bool zcashlc_update_chain_tip(const uint8_t *db_data,
                              uintptr_t db_data_len,
                              int32_t height,
                              uint32_t network_id);

/**
 * Returns the height to which the wallet has been fully scanned.
 *
 * This is the height for which the wallet has fully trial-decrypted this and all
 * preceding blocks above the wallet's birthday height.
 *
 * Returns a non-negative block height, -1 if empty, or -2 if an error occurred.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 */
int64_t zcashlc_fully_scanned_height(const uint8_t *db_data,
                                     uintptr_t db_data_len,
                                     uint32_t network_id);

/**
 * Returns the maximum height that the wallet has scanned.
 *
 * If the wallet is fully synced, this will be equivalent to `zcashlc_block_fully_scanned`;
 * otherwise the maximal scanned height is likely to be greater than the fully scanned
 * height due to the fact that out-of-order scanning can leave gaps.
 *
 * Returns a non-negative block height, -1 if empty, or -2 if an error occurred.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 */
int64_t zcashlc_max_scanned_height(const uint8_t *db_data,
                                   uintptr_t db_data_len,
                                   uint32_t network_id);

/**
 * Returns the account balances and sync status given the specified minimum number of
 * confirmations.
 *
 * Returns `fully_scanned_height = -1` if the wallet has no balance data available.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must
 *   have an alignment of `1`. Its contents must be a string representing a valid system
 *   path in the operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the
 *   function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
struct FfiWalletSummary *zcashlc_get_wallet_summary(const uint8_t *db_data,
                                                    uintptr_t db_data_len,
                                                    uint32_t network_id,
                                                    uint32_t min_confirmations);

/**
 * Frees an [`FfiWalletSummary`] value.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FfiWalletSummary`].
 *   See the safety documentation of [`FfiWalletSummary`].
 */
void zcashlc_free_wallet_summary(struct FfiWalletSummary *ptr);

/**
 * Frees an array of `FfiScanRanges` values as allocated by `zcashlc_suggest_scan_ranges`.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FfiScanRanges`].
 *   See the safety documentation of [`FfiScanRanges`].
 */
void zcashlc_free_scan_ranges(struct FfiScanRanges *ptr);

/**
 * Returns a list of suggested scan ranges based upon the current wallet state.
 *
 * This method should only be used in cases where the `CompactBlock` data that will be
 * made available to `zcashlc_scan_blocks` for the requested block ranges includes note
 * commitment tree size information for each block; or else the scan is likely to fail if
 * notes belonging to the wallet are detected.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must
 *   have an alignment of `1`. Its contents must be a string representing a valid system
 *   path in the operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the
 *   function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_free_scan_ranges`] to free the memory associated with the returned
 *   pointer when done using it.
 */
struct FfiScanRanges *zcashlc_suggest_scan_ranges(const uint8_t *db_data,
                                                  uintptr_t db_data_len,
                                                  uint32_t network_id);

/**
 * Scans new blocks added to the cache for any transactions received by the tracked
 * accounts, while checking that they form a valid chan.
 *
 * This function is built on the core assumption that the information provided in the
 * block cache is more likely to be accurate than the previously-scanned information.
 * This follows from the design (and trust) assumption that the `lightwalletd` server
 * provides accurate block information as of the time it was requested.
 *
 * This function **assumes** that the caller is handling rollbacks.
 *
 * For brand-new light client databases, this function starts scanning from the Sapling
 * activation height. This height can be fast-forwarded to a more recent block by calling
 * [`zcashlc_init_blocks_table`] before this function.
 *
 * Scanned blocks are required to be height-sequential. If a block is missing from the
 * cache, an error will be signalled.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
struct FfiScanSummary *zcashlc_scan_blocks(const uint8_t *fs_block_cache_root,
                                           uintptr_t fs_block_cache_root_len,
                                           const uint8_t *db_data,
                                           uintptr_t db_data_len,
                                           int32_t from_height,
                                           const uint8_t *from_state,
                                           uintptr_t from_state_len,
                                           uint32_t scan_limit,
                                           uint32_t network_id);

/**
 * Frees an [`FfiScanSummary`] value.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FfiScanSummary`].
 */
void zcashlc_free_scan_summary(struct FfiScanSummary *ptr);

/**
 * Inserts a UTXO into the wallet database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `txid_bytes` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `txid_bytes_len` must not be mutated for the duration of the function call.
 * - The total size `txid_bytes_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `script_bytes` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `script_bytes_len` must not be mutated for the duration of the function call.
 * - The total size `script_bytes_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool zcashlc_put_utxo(const uint8_t *db_data,
                      uintptr_t db_data_len,
                      const uint8_t *txid_bytes,
                      uintptr_t txid_bytes_len,
                      int32_t index,
                      const uint8_t *script_bytes,
                      uintptr_t script_bytes_len,
                      int64_t value,
                      int32_t height,
                      uint32_t network_id);

/**
 * # Safety
 * Initializes the `FsBlockDb` sqlite database. Does nothing if already created
 *
 * Returns true when successful, false otherwise. When false is returned caller
 * should check for errors.
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool zcashlc_init_block_metadata_db(const uint8_t *fs_block_db_root,
                                    uintptr_t fs_block_db_root_len);

/**
 * Writes the blocks provided in `blocks_meta` into the `BlockMeta` database
 *
 * Returns true if the `blocks_meta` could be stored into the `FsBlockDb`. False
 * otherwise.
 *
 * When false is returned caller should check for errors.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Block metadata represented in `blocks_meta` must be non-null. Caller must guarantee that the
 * memory reference by this pointer is not freed up, dereferenced or invalidated while this function
 * is invoked.
 */
bool zcashlc_write_block_metadata(const uint8_t *fs_block_db_root,
                                  uintptr_t fs_block_db_root_len,
                                  struct FFIBlocksMeta *blocks_meta);

/**
 * Rewinds the data database to the given height.
 *
 * If the requested height is greater than or equal to the height of the last scanned
 * block, this function does nothing.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool zcashlc_rewind_fs_block_cache_to_height(const uint8_t *fs_block_db_root,
                                             uintptr_t fs_block_db_root_len,
                                             int32_t height);

/**
 * Get the latest cached block height in the filesystem block cache
 *
 * Returns a non-negative block height, -1 if empty, or -2 if an error occurred.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `tx` must be non-null and valid for reads for `tx_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `tx` must not be mutated for the duration of the function call.
 * - The total size `tx_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t zcashlc_latest_cached_block_height(const uint8_t *fs_block_db_root,
                                           uintptr_t fs_block_db_root_len);

/**
 * Decrypts whatever parts of the specified transaction it can and stores them in db_data.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `tx` must be non-null and valid for reads for `tx_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `tx` must not be mutated for the duration of the function call.
 * - The total size `tx_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t zcashlc_decrypt_and_store_transaction(const uint8_t *db_data,
                                              uintptr_t db_data_len,
                                              const uint8_t *tx,
                                              uintptr_t tx_len,
                                              int64_t mined_height,
                                              uint32_t network_id);

/**
 * Frees an [`FfiBoxedSlice`].
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of
 *   [`FfiBoxedSlice`]. See the safety documentation of [`FfiBoxedSlice`].
 */
void zcashlc_free_boxed_slice(struct FfiBoxedSlice *ptr);

/**
 * Select transaction inputs, compute fees, and construct a proposal for a transaction
 * that can then be authorized and made ready for submission to the network with
 * `zcashlc_create_proposed_transaction`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `to` must be non-null and must point to a null-terminated UTF-8 string.
 * - `memo` must either be null (indicating an empty memo or a transparent recipient) or point to a
 *    512-byte array.
 * - Call [`zcashlc_free_boxed_slice`] to free the memory associated with the returned
 *   pointer when done using it.
 */
struct FfiBoxedSlice *zcashlc_propose_transfer(const uint8_t *db_data,
                                               uintptr_t db_data_len,
                                               int32_t account,
                                               const char *to,
                                               int64_t value,
                                               const uint8_t *memo,
                                               uint32_t network_id,
                                               uint32_t min_confirmations);

/**
 * Select transaction inputs, compute fees, and construct a proposal for a transaction
 * from a ZIP-321 payment URI that can then be authorized and made ready for submission to the
 * network with `zcashlc_create_proposed_transaction`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `payment_uri` must be non-null and must point to a null-terminated UTF-8 string.
 * - `network_id` a u32. 0 for Testnet and 1 for Mainnet
 * - `min_confirmations` number of confirmations of the funds to spend
 * - `use_zip317_fees` `true` to use ZIP-317 fees.
 * - Call [`zcashlc_free_boxed_slice`] to free the memory associated with the returned
 *   pointer when done using it.
 */
struct FfiBoxedSlice *zcashlc_propose_transfer_from_uri(const uint8_t *db_data,
                                                        uintptr_t db_data_len,
                                                        int32_t account,
                                                        const char *payment_uri,
                                                        uint32_t network_id,
                                                        uint32_t min_confirmations);

int32_t zcashlc_branch_id_for_height(int32_t height, uint32_t network_id);

/**
 * Frees strings returned by other zcashlc functions.
 *
 * # Safety
 *
 * - `s` should be a non-null pointer returned as a string by another zcashlc function.
 */
void zcashlc_string_free(char *s);

/**
 * Select transaction inputs, compute fees, and construct a proposal for a shielding
 * transaction that can then be authorized and made ready for submission to the network
 * with `zcashlc_create_proposed_transaction`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `shielding_threshold` a non-negative shielding threshold amount in zatoshi
 * - Call [`zcashlc_free_boxed_slice`] to free the memory associated with the returned
 *   pointer when done using it.
 */
struct FfiBoxedSlice *zcashlc_propose_shielding(const uint8_t *db_data,
                                                uintptr_t db_data_len,
                                                int32_t account,
                                                const uint8_t *memo,
                                                uint64_t shielding_threshold,
                                                const char *transparent_receiver,
                                                uint32_t network_id,
                                                uint32_t min_confirmations);

/**
 * Frees an array of FfiTxIds values as allocated by `zcashlc_create_proposed_transactions`.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FfiTxIds`].
 *   See the safety documentation of [`FfiTxIds`].
 */
void zcashlc_free_txids(struct FfiTxIds *ptr);

/**
 * Creates a transaction from the given proposal.
 *
 * Returns the row index of the newly-created transaction in the `transactions` table
 * within the data database. The caller can read the raw transaction bytes from the `raw`
 * column in order to broadcast the transaction to the network.
 *
 * Do not call this multiple times in parallel, or you will generate transactions that
 * double-spend the same notes.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must
 *   have an alignment of `1`. Its contents must be a string representing a valid system
 *   path in the operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the
 *   function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `proposal_ptr` must be non-null and valid for reads for `proposal_len` bytes, and it
 *   must have an alignment of `1`. Its contents must be an encoded Proposal protobuf.
 * - The memory referenced by `proposal_ptr` must not be mutated for the duration of the
 *   function call.
 * - The total size `proposal_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `usk_ptr` must be non-null and must point to an array of `usk_len` bytes containing
 *   a unified spending key encoded as returned from the `zcashlc_create_account` or
 *   `zcashlc_derive_spending_key` functions.
 * - The memory referenced by `usk_ptr` must not be mutated for the duration of the
 *   function call.
 * - The total size `usk_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `to` must be non-null and must point to a null-terminated UTF-8 string.
 * - `memo` must either be null (indicating an empty memo or a transparent recipient) or
 *   point to a 512-byte array.
 * - `spend_params` must be non-null and valid for reads for `spend_params_len` bytes,
 *   and it must have an alignment of `1`. Its contents must be the Sapling spend proving
 *   parameters.
 * - The memory referenced by `spend_params` must not be mutated for the duration of the
 *   function call.
 * - The total size `spend_params_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `output_params` must be non-null and valid for reads for `output_params_len` bytes,
 *   and it must have an alignment of `1`. Its contents must be the Sapling output
 *   proving parameters.
 * - The memory referenced by `output_params` must not be mutated for the duration of the
 *   function call.
 * - The total size `output_params_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
struct FfiTxIds *zcashlc_create_proposed_transactions(const uint8_t *db_data,
                                                      uintptr_t db_data_len,
                                                      const uint8_t *proposal_ptr,
                                                      uintptr_t proposal_len,
                                                      const uint8_t *usk_ptr,
                                                      uintptr_t usk_len,
                                                      const uint8_t *spend_params,
                                                      uintptr_t spend_params_len,
                                                      const uint8_t *output_params,
                                                      uintptr_t output_params_len,
                                                      uint32_t network_id);

/**
 * Sets the transaction status to the provided value.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must
 *   have an alignment of `1`. Its contents must be a string representing a valid system
 *   path in the operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the
 *   function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `txid_bytes` must be non-null and valid for reads for `db_data_len` bytes, and it must have
 *   an alignment of `1`.
 * - The memory referenced by `txid_bytes_len` must not be mutated for the duration of the
 *   function call.
 * - The total size `txid_bytes_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
void zcashlc_set_transaction_status(const uint8_t *db_data,
                                    uintptr_t db_data_len,
                                    uint32_t network_id,
                                    const uint8_t *txid_bytes,
                                    uintptr_t txid_bytes_len,
                                    struct FfiTransactionStatus status);

/**
 * Frees an array of FfiTransactionDataRequest values as allocated by `zcashlc_transaction_data_requests`.
 *
 * # Safety
 *
 * - `ptr` if `ptr` is non-null it must point to a struct having the layout of [`FfiTransactionDataRequests`].
 *   See the safety documentation of [`FfiTransactionDataRequests`].
 */
void zcashlc_free_transaction_data_requests(struct FfiTransactionDataRequests *ptr);

/**
 * Returns a list of transaction data requests that the network client should satisfy.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_free_transaction_data_requests`] to free the memory associated with the
 *   returned pointer when done using it.
 */
struct FfiTransactionDataRequests *zcashlc_transaction_data_requests(const uint8_t *db_data,
                                                                     uintptr_t db_data_len,
                                                                     uint32_t network_id);

/**
 * Creates a Tor runtime.
 *
 * # Safety
 *
 * - `tor_dir` must be non-null and valid for reads for `tor_dir_len` bytes, and it must
 *   have an alignment of `1`. Its contents must be a string representing a valid system
 *   path in the operating system's preferred representation.
 * - The memory referenced by `tor_dir` must not be mutated for the duration of the
 *   function call.
 * - The total size `tor_dir_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`zcashlc_free_tor_runtime`] to free the memory associated with the returned
 *   pointer when done using it.
 */
struct TorRuntime *zcashlc_create_tor_runtime(const uint8_t *tor_dir, uintptr_t tor_dir_len);

/**
 * Frees a Tor runtime.
 *
 * # Safety
 *
 * - If `ptr` is non-null, it must point to a struct having the layout of [`TorRuntime`].
 */
void zcashlc_free_tor_runtime(struct TorRuntime *ptr);

/**
 * Fetches the current ZEC-USD exchange rate over Tor.
 *
 * The result is a [`Decimal`] struct containing the fields necessary to construct an
 * [`NSDecimalNumber`](https://developer.apple.com/documentation/foundation/nsdecimalnumber/1416003-init).
 *
 * Returns a negative value on error.
 *
 * # Safety
 *
 * - `tor_runtime` must be non-null and point to a struct having the layout of
 *   [`TorRuntime`].
 * - `tor_runtime` must not be passed to two FFI calls at the same time.
 */
struct Decimal zcashlc_get_exchange_rate_usd(struct TorRuntime *tor_runtime);

/**
 * Returns the network type and address kind for the given address string,
 * if the address is a valid Zcash address.
 *
 * Address kind codes are as follows:
 * * p2pkh: 0
 * * p2sh: 1
 * * sapling: 2
 * * unified: 3
 * * tex: 4
 *
 * # Safety
 *
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
bool zcashlc_get_address_metadata(const char *address,
                                  uint32_t *network_id_ret,
                                  uint32_t *addr_kind_ret);

/**
 * Extracts the typecodes of the receivers within the given Unified Address.
 *
 * Returns a pointer to a slice of typecodes. `len_ret` is set to the length of the
 * slice.
 *
 * See the following sections of ZIP 316 for details on how to interpret typecodes:
 * - [List of known typecodes](https://zips.z.cash/zip-0316#encoding-of-unified-addresses)
 * - [Adding new types](https://zips.z.cash/zip-0316#adding-new-types)
 * - [Metadata Items](https://zips.z.cash/zip-0316#metadata-items)
 *
 * # Safety
 *
 * - `ua` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `ua` must not be mutated for the duration of the function call.
 * - Call [`zcashlc_free_typecodes`] to free the memory associated with the returned
 *   pointer when done using it.
 */
uint32_t *zcashlc_get_typecodes_for_unified_address_receivers(const char *ua, uintptr_t *len_ret);

/**
 * Frees a list of typecodes previously obtained from the FFI.
 *
 * # Safety
 *
 * - `data` and `len` must have been obtained from
 *   [`zcashlc_get_typecodes_for_unified_address_receivers`].
 */
void zcashlc_free_typecodes(uint32_t *data, uintptr_t len);

/**
 * Returns true when the provided key decodes to a valid Sapling extended spending key for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `extsk` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `extsk` must not be mutated for the duration of the function call.
 */
bool zcashlc_is_valid_sapling_extended_spending_key(const char *extsk, uint32_t network_id);

/**
 * Returns true when the provided key decodes to a valid Sapling extended full viewing key for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `key` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `key` must not be mutated for the duration of the function call.
 */
bool zcashlc_is_valid_viewing_key(const char *key, uint32_t network_id);

/**
 * Returns true when the provided key decodes to a valid unified full viewing key for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `ufvk` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `ufvk` must not be mutated for the duration of the
 *   function call.
 */
bool zcashlc_is_valid_unified_full_viewing_key(const char *ufvk, uint32_t network_id);

/**
 * Derives and returns a unified spending key from the given seed for the given account ID.
 *
 * Returns the binary encoding of the spending key. The caller should manage the memory of (and
 * store, if necessary) the returned spending key in a secure fashion.
 *
 * # Safety
 *
 * - `seed` must be non-null and valid for reads for `seed_len` bytes.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of `pointer::offset`.
 * - Call `zcashlc_free_binary_key` to free the memory associated with the returned pointer when
 *   you are finished using it.
 */
struct FFIBinaryKey *zcashlc_derive_spending_key(const uint8_t *seed,
                                                 uintptr_t seed_len,
                                                 int32_t account,
                                                 uint32_t network_id);

/**
 * Obtains the unified full viewing key for the given binary-encoded unified spending key
 * and returns the resulting encoded UFVK string. `usk_ptr` should point to an array of `usk_len`
 * bytes containing a unified spending key encoded as returned from the `zcashlc_create_account`
 * or `zcashlc_derive_spending_key` functions.
 *
 * # Safety
 *
 * - `usk_ptr` must be non-null and must point to an array of `usk_len` bytes.
 * - The memory referenced by `usk_ptr` must not be mutated for the duration of the function call.
 * - The total size `usk_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of `pointer::offset`.
 * - Call [`zcashlc_string_free`] to free the memory associated with the returned pointer
 *   when you are done using it.
 */
char *zcashlc_spending_key_to_full_viewing_key(const uint8_t *usk_ptr,
                                               uintptr_t usk_len,
                                               uint32_t network_id);

/**
 * Returns the transparent receiver within the given Unified Address, if any.
 *
 * # Safety
 *
 * - `ua` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `ua` must not be mutated for the duration of the function call.
 * - Call [`zcashlc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *zcashlc_get_transparent_receiver_for_unified_address(const char *ua);

/**
 * Returns the Sapling receiver within the given Unified Address, if any.
 *
 * # Safety
 *
 * - `ua` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `ua` must not be mutated for the duration of the function call.
 * - Call [`zcashlc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *zcashlc_get_sapling_receiver_for_unified_address(const char *ua);

/**
 * Derives and returns a ZIP 32 Arbitrary Key from the given seed at the "wallet level", i.e.
 * directly from the seed with no ZIP 32 path applied.
 *
 * The resulting key will be the same across all networks (Zcash mainnet, Zcash testnet, OtherCoin
 * mainnet, and so on). You can think of it as a context-specific seed fingerprint that can be used
 * as (static) key material.
 *
 * `context_string` is a globally-unique non-empty sequence of at most 252 bytes that identifies
 * the desired context.
 *
 * # Safety
 *
 * - `context_string` must be non-null and valid for reads for `context_string_len` bytes.
 * - The memory referenced by `context_string` must not be mutated for the duration of the function
 *   call.
 * - The total size `context_string_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of `pointer::offset`.
 * - Call `zcashlc_free_boxed_slice` to free the memory associated with the returned
 *   pointer when done using it.
 */
struct FfiBoxedSlice *zcashlc_derive_arbitrary_wallet_key(const uint8_t *context_string,
                                                          uintptr_t context_string_len,
                                                          const uint8_t *seed,
                                                          uintptr_t seed_len);

/**
 * Derives and returns a ZIP 32 Arbitrary Key from the given seed at the account level.
 *
 * `context_string` is a globally-unique non-empty sequence of at most 252 bytes that identifies
 * the desired context.
 *
 * # Safety
 *
 * - `context_string` must be non-null and valid for reads for `context_string_len` bytes.
 * - The memory referenced by `context_string` must not be mutated for the duration of the function
 *   call.
 * - The total size `context_string_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of `pointer::offset`.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of `pointer::offset`.
 * - Call `zcashlc_free_boxed_slice` to free the memory associated with the returned
 *   pointer when done using it.
 */
struct FfiBoxedSlice *zcashlc_derive_arbitrary_account_key(const uint8_t *context_string,
                                                           uintptr_t context_string_len,
                                                           const uint8_t *seed,
                                                           uintptr_t seed_len,
                                                           int32_t account,
                                                           uint32_t network_id);
